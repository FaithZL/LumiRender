//
// Created by Zero on 2021/3/24.
//

#include "integrator.h"
#include "embree_aggregate.h"
#include "gpu/accel/optix_aggregate.h"
#include "util/progressreporter.h"

extern "C" char wavefront_kernels[];

namespace luminous {
    inline namespace gpu {

        WavefrontPT::WavefrontPT(Device *device, Context *context)
                : Integrator(device, context) {

        }

        void WavefrontPT::load_module() {
            if (_device->is_cpu()) {
                return;
            }
            _module = create_cuda_module(wavefront_kernels);
        }

        void WavefrontPT::init(const std::shared_ptr<SceneGraph> &scene_graph) {
            Integrator::init(scene_graph);
            // todo: make this configurable. Base it on the amount of GPU memory?
            int max_samples = 1024 * 1024;
            uint2 res = resolution();
            _scanline_per_pass = std::max(1, int(max_samples / res.x));
            auto n_passes = (res.y + _scanline_per_pass - 1) / _scanline_per_pass;
            _scanline_per_pass = (res.y + n_passes - 1) / n_passes;
            _max_queue_size = res.x * _scanline_per_pass;
            allocate_memory();
            init_aggregate();
            load_module();
            init_rt_param();
            init_kernels();
        }

        void WavefrontPT::allocate_memory() {
            _ray_queues.emplace_back(_max_queue_size, _device);
            _ray_queues.emplace_back(_max_queue_size, _device);
            _ray_queues.allocate_device();
            _ray_queues.synchronize_to_device();

#define ALLOCATE_AND_SYNCHRONIZE(args)         \
(args).emplace_back(_max_queue_size, _device); \
(args).allocate_device();                      \
(args).synchronize_to_device();
            ALLOCATE_AND_SYNCHRONIZE(_shadow_ray_queue)
            ALLOCATE_AND_SYNCHRONIZE(_hit_area_light_queue)
            ALLOCATE_AND_SYNCHRONIZE(_escaped_ray_queue)
            ALLOCATE_AND_SYNCHRONIZE(_material_eval_queue)
            ALLOCATE_AND_SYNCHRONIZE(_pixel_sample_state)
#undef ALLOCATE_AND_SYNCHRONIZE

        }

        void WavefrontPT::render_per_sample(int sample_idx, int spp) {
            auto res = _camera->resolution();
            for (int y0 = 0; y0 < res.y; y0 += _scanline_per_pass) {
                _reset_ray_queue(0);
                _generate_primary_ray.launch(*_dispatcher, _max_queue_size, y0, sample_idx,
                                             _ray_queues.device_data(),
                                             _pixel_sample_state.device_data());
                check_wait();

                for (int depth = 0; true; ++depth) {
                    reset_queues(depth);
                    RayQueue *cur_ray_queue = _current_ray_queue(depth);
                    _generate_ray_samples.launch(*_dispatcher, _max_queue_size,
                                                 sample_idx, cur_ray_queue,
                                                 _pixel_sample_state.device_data());
                    check_wait();

                    intersect_closest(depth);

                    check_wait();

                    _process_escape_ray.launch(*_dispatcher, _max_queue_size,
                                               _escaped_ray_queue.device_data(),
                                               _pixel_sample_state.device_data());
                    check_wait();

                    _process_emission.launch(*_dispatcher, _max_queue_size,
                                             _hit_area_light_queue.device_data(),
                                             _pixel_sample_state.device_data());
                    check_wait();

                    BREAK_IF(depth == _max_depth)

                    RayQueue *next_ray_queue = _next_ray_queue(depth);

                    _estimate_direct_lighting.launch(*_dispatcher, _max_queue_size,
                                                     _shadow_ray_queue.device_data(),
                                                     next_ray_queue,
                                                     _material_eval_queue.device_data(),
                                                     _pixel_sample_state.device_data());
                    check_wait();

                    intersect_any_and_compute_lighting(depth);

                    check_wait();
                }
            }
        }

        void WavefrontPT::render(int frame_num, ProgressReporter *progressor) {
            auto spp = _sampler->spp();
            for (int sample_idx = 0; sample_idx < spp; ++sample_idx) {
                render_per_sample(_rt_param->frame_index, spp);
                if (progressor) progressor->update(1);
            }
            _add_samples.launch(*_dispatcher, _max_queue_size,
                                _pixel_sample_state.device_data());
            _dispatcher->wait();
            _rt_param->frame_index += 1;
            _rt_param.synchronize_to_device();
        }

        void WavefrontPT::reset_queues(int depth) {
            _reset_next_ray_queue(depth);

            _shadow_ray_queue->reset();
            _shadow_ray_queue.synchronize_to_device();

            _escaped_ray_queue->reset();
            _escaped_ray_queue.synchronize_to_device();

            _hit_area_light_queue->reset();
            _hit_area_light_queue.synchronize_to_device();

            _material_eval_queue->reset();
            _material_eval_queue.synchronize_to_device();
        }

        void WavefrontPT::init_kernels() {
            if (_device->is_cpu()) {
                return;
            }

#define SET_CU_FUNC(arg) _##arg.set_cu_function(_module->get_kernel_handle("kernel_"#arg));
            SET_CU_FUNC(generate_primary_ray);
            SET_CU_FUNC(generate_ray_samples);
            SET_CU_FUNC(process_escape_ray);
            SET_CU_FUNC(process_emission);
            SET_CU_FUNC(estimate_direct_lighting);
            SET_CU_FUNC(add_samples);
#undef SET_CU_FUNC

        }

        void WavefrontPT::init_rt_param() {
            _rt_param.push_back({_sampler.device_data(), _camera.device_ptr(),
                                 0, *_scene->scene_data_host_ptr()});
            _rt_param.allocate_device(1);
            _rt_param.synchronize_to_device();
            if (_device->is_cpu()) {
                set_rt_param(_rt_param.data());
            } else {
                auto data = _rt_param.device_data();
                _module->upload_data_to_global_var("rt_param", &data);
            }
        }

        void WavefrontPT::init_aggregate() {
            if (_device->is_cpu()) {
                _scene->init_accel<EmbreeAggregate>();
            } else {
                _scene->init_accel<OptixAggregate>();
            }
            _aggregate = _scene->accel<WavefrontAggregate>();
        }

        void WavefrontPT::intersect_closest(int depth) {
            _aggregate->intersect_closest(_max_queue_size, _current_ray_queue(depth),
                                          _escaped_ray_queue.device_data(),
                                          _hit_area_light_queue.device_data(),
                                          _material_eval_queue.device_data(),
                                          _next_ray_queue(depth));
        }

        void WavefrontPT::intersect_any_and_compute_lighting(int depth) {
            _aggregate->intersect_any_and_compute_lighting(_max_queue_size,
                                                           _shadow_ray_queue.device_data(),
                                                           _pixel_sample_state.device_data());
        }

        void WavefrontPT::check_wait() {
            if (_device->is_cpu()) {
                _dispatcher->wait();
            }
        }
    }
}