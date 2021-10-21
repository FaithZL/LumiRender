//
// Created by Zero on 2021/3/24.
//

#include "integrator.h"
#include "cpu/accel/embree_aggregate.h"
#include "gpu/accel/optix_aggregate.h"

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

        void WavefrontPT::render_per_sample(int sample_idx) {
            auto res = _camera->resolution();
            for (int y0 = 0; y0 < res.y; y0 += _scanline_per_pass) {
                
            }
        }

        void WavefrontPT::render() {
            auto spp = _sampler->spp();
            for (int sample_idx = 0; sample_idx < spp; ++sample_idx) {
                render_per_sample(sample_idx);
            }
        }

        void WavefrontPT::init_kernels() {
            if (_device->is_cpu()) {
                return;
            }

#define SET_CU_FUNC(arg) _##arg.set_cu_function(_module->get_kernel_handle("kernel_"#arg));
            SET_CU_FUNC(generate_primary_ray);
            SET_CU_FUNC(reset_ray_queue);
            SET_CU_FUNC(reset_queues);
            SET_CU_FUNC(generate_ray_samples);
            SET_CU_FUNC(process_escape_ray);
            SET_CU_FUNC(process_emission);
            SET_CU_FUNC(eval_BSDFs);
#define undef SET_CU_FUNC

        }

        void WavefrontPT::init_rt_param() {
            if (_device->is_cpu()) {
                _rt_param.emplace_back(_sampler.data(), _camera.data());
            } else {
                auto [ptr, size] = _module->get_global_var("rt_param");
                _rt_param.emplace_back(_sampler.device_data(), _camera.device_ptr());
            }
            _rt_param.allocate_device(1);
            _rt_param.synchronize_to_device();
        }

        void WavefrontPT::init_aggregate() {
            if (_device->is_cpu()) {
                _scene->init_accel<EmbreeAggregate>();
            } else {
                _scene->init_accel<OptixAggregate>();
            }
            _aggregate = _scene->accel<WavefrontAggregate>();
        }

        void WavefrontPT::intersect_closest(int wavefront_depth) {
            _aggregate->intersect_closest(_max_queue_size, current_ray_queue(wavefront_depth),
                                          _escaped_ray_queue.device_data(),
                                          _hit_area_light_queue.device_data(),
                                          _material_eval_queue.device_data(),
                                          next_ray_queue(wavefront_depth));
        }

        void WavefrontPT::trace_shadow_ray(int wavefront_depth) {
            _aggregate->intersect_any(_max_queue_size, _shadow_ray_queue.device_data(),
                                      _pixel_sample_state.device_data());
        }

    }
}