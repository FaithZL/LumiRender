//
// Created by Zero on 12/10/2021.
//

#include "task.h"
#include "render/integrators/megakernel_pt.h"
#include "render/integrators/cpu_pt.h"
#include "render/integrators/wavefront/integrator.h"
#include <core/film_denoiser.h>
#include <gpu/film_optix_denoiser.h>

namespace luminous {
    inline namespace render {
        void Task::on_key(int key, int scancode, int action, int mods) {
            auto p_camera = camera();
            float3 forward = p_camera->forward();
            float3 up = p_camera->up();
            float3 right = p_camera->right();
            float distance = p_camera->velocity() * _dt;
            switch (key) {
                case 'A':
                    p_camera->move(-right * distance);
                    break;
                case 'S':
                    p_camera->move(-forward * distance);
                    break;
                case 'D':
                    p_camera->move(right * distance);
                    break;
                case 'W':
                    p_camera->move(forward * distance);
                    break;
                case 'Q':
                    p_camera->move(-up * distance);
                    break;
                case 'E':
                    p_camera->move(up * distance);
                    break;
                default:
                    break;
            }
        }

        void Task::update_camera_view(float d_yaw, float d_pitch) {
            float sensitivity = camera()->sensitivity();
            camera()->update_yaw(d_yaw * sensitivity);
            camera()->update_pitch(d_pitch * sensitivity);
        }

        uint2 Task::resolution() {
            return camera()->film()->resolution();
        }

        void Task::update_camera_fov_y(float val) {
            camera()->update_fov_y(val);
        }

        void Task::update_film_resolution(uint2 res) {
            camera()->update_film_resolution(res);
            update_device_buffer();
        }

        void Task::update_device_buffer() {
            auto res = camera()->film()->resolution();
            auto num = res.x * res.y;

            _render_buffer.resize(num, make_float4(0.f));
            _render_buffer.allocate_device(num);
            camera()->film()->set_render_buffer_view(_render_buffer.device_buffer_view());

            _normal_buffer.resize(num, make_float4(0.f));
            _normal_buffer.allocate_device(num);
            camera()->film()->set_normal_buffer_view(_normal_buffer.device_buffer_view());

            _albedo_buffer.resize(num, make_float4(0.f));
            _albedo_buffer.allocate_device(num);
            camera()->film()->set_albedo_buffer_view(_albedo_buffer.device_buffer_view());

            _frame_buffer.reset(num);
            _frame_buffer.synchronize_to_device();
            camera()->film()->set_frame_buffer_view(_frame_buffer.device_buffer_view());

        }

        FrameBufferType *Task::get_frame_buffer(bool host_side) {
            if(host_side)
                return _frame_buffer.synchronize_and_get_host_data();
            else
                return _frame_buffer.device_data();
        }

        float4 *Task::get_render_buffer(bool host_side) {
            if(host_side)
                return _render_buffer.synchronize_and_get_host_data();
            else
                return _render_buffer.device_data();
        }

        float4 *Task::get_normal_buffer(bool host_side) {
            if(host_side)
                return _normal_buffer.synchronize_and_get_host_data();
            else
                return _normal_buffer.device_data();
        }

        float4 *Task::get_albedo_buffer(bool host_side) {
            if(host_side)
                return _albedo_buffer.synchronize_and_get_host_data();
            else
                return _albedo_buffer.device_data();
        }

        float4 *Task::get_buffer(bool host_side) {
            auto fb_state = camera()->film()->frame_buffer_state();
            switch (fb_state) {
                case Render:
                    return get_render_buffer(host_side);
                case Normal:
                    return get_normal_buffer(host_side);
                case Albedo:
                    return get_albedo_buffer(host_side);
            }
            DCHECK(0);
            return nullptr;
        }

        void Task::init(const Parser &parser) {
            auto scene_graph = build_scene_graph(parser);
            const std::string type = scene_graph->integrator_config.type();
            if (type == "PT") {
                if (_device->is_cpu()) {
                    _integrator = std::make_unique<CPUPathTracer>(_device.get(), _context);
                } else {
                    _integrator = std::make_unique<MegakernelPT>(_device.get(), _context);
                }
            } else if (type == "WavefrontPT") {
                _integrator = std::make_unique<WavefrontPT>(_device.get(), _context);
            }
            _integrator->init(scene_graph);
            update_device_buffer();
            _clock.reset();
        }

        std::shared_ptr<SceneGraph> Task::build_scene_graph(const Parser &parser) {
            auto scene_graph = parser.parse();
            _output_config = scene_graph->output_config;
            return scene_graph;
        }

        void Task::save_to_file() {
            float4 *buffer = get_buffer();
            auto res = resolution();
            size_t size = res.x * res.y * pixel_size(PixelFormat::RGBA32F);
            auto p = new std::byte[size];
            Image image = Image(PixelFormat::RGBA32F, p, res);
            image.for_each_pixel([&](std::byte *pixel, int i) {
                auto fp = reinterpret_cast<float4 *>(pixel);
                float4 val = buffer[i];
                *fp = Spectrum::linear_to_srgb(val / val.w);
            });

            // denoising
            if(_context->denoise_output()) {
                auto denoiser = create_film_optix_denoiser();
                std::unique_ptr<float4[]> denoise_output_buffer{new float4[res.x * res.y]};
                denoiser->init_device();

                bool is_gpu_rendering = _context->use_gpu();
                const FilmDenoiserBufferViewType denoise_bv_type = is_gpu_rendering ? FILMDENOISER_BUFFER_VIEW_TYPE_CUDA_DEVICE_MEM
                                                                                    : FILMDENOISER_BUFFER_VIEW_TYPE_HOST_MEM;

                FilmDenoiserInputData data;
                data.width = res.x;
                data.height = res.y;
                data.color.type = FILMDENOISER_BUFFER_VIEW_TYPE_HOST_MEM;
                data.color.format = FILMDENOISER_PIXEL_FORMAT_FLOAT4;
                data.color.address = reinterpret_cast<unsigned long long>(p);
                data.albedo.type = denoise_bv_type;
                data.albedo.format = FILMDENOISER_PIXEL_FORMAT_FLOAT4;
                data.albedo.address = reinterpret_cast<unsigned long long>(this->get_albedo_buffer(!is_gpu_rendering));
                data.normal.type = denoise_bv_type;
                data.normal.format = FILMDENOISER_PIXEL_FORMAT_FLOAT4;
                data.normal.address = reinterpret_cast<unsigned long long>(this->get_normal_buffer(!is_gpu_rendering));
                data.flow.type = FILMDENOISER_BUFFER_VIEW_TYPE_HOST_MEM;
                data.flow.format = FILMDENOISER_PIXEL_FORMAT_FLOAT4;

                data.output.type = FILMDENOISER_BUFFER_VIEW_TYPE_HOST_MEM;
                data.output.format = FILMDENOISER_PIXEL_FORMAT_FLOAT4;
                data.output.address = reinterpret_cast<unsigned long long>(denoise_output_buffer.get());

                denoiser->init_context(data, res.x, res.y, false);

                denoiser->exec();
                denoiser->get_results();

                std::byte *raw_buffer = reinterpret_cast<std::byte *>(denoise_output_buffer.release());
                auto image2 = Image(PixelFormat::RGBA32F, raw_buffer, res);

                luminous_fs::path path = _context->scene_path() / _output_config.fn;
                image2.save(path);
            } else {
                luminous_fs::path path = _context->scene_path() / _output_config.fn;
                image.save(path);
            }
        }

        void Task::render_gui(double dt) {
            _dt = dt;
            _integrator->render(_output_config.frame_per_dispatch);
            if (_dispatch_num++ == _output_config.dispatch_num
                && _output_config.dispatch_num != 0) {
                auto t = _clock.get_time();
                cout << "render is complete elapse " << t << " s" << endl;
                cout << (_output_config.dispatch_num * _output_config.frame_per_dispatch) / t << " fps" << endl;
                save_to_file();
            }
        }
    }
}