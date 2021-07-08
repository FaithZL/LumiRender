//
// Created by Zero on 2021/3/23.
//


#pragma once

#include "render/include/integrator.h"
#include "gpu/framework/optix_accel.h"
#include "render/samplers/sampler.h"

namespace luminous {

    inline namespace gpu {
        class GPUScene;

        class MegakernelPT : public Integrator {
        private:
            Context *_context{};
            Managed<Sampler, Sampler> _sampler;
            Managed<Sensor, Sensor> _camera;
            UP<GPUScene> _scene{nullptr};
            SP<Device> _device{};
            Managed<LaunchParams> _launch_params;
        public:

            MegakernelPT(const SP<Device> &device, Context *context);

            void test() override {
                auto film = _camera->film();
                _launch_params->accum_buffer = reinterpret_cast<::float4*>(film->accumulate_buffer_ptr());
                _launch_params->frame_buffer = reinterpret_cast<FrameBufferType*>(film->frame_buffer_ptr());
            }

            void init(const SP<SceneGraph> &scene_graph) override;

            void init_launch_params();

            Sensor *camera() override;

            void update() override;

            void synchronize_to_gpu();

            void render() override;

            void update_camera_fov_y(float val);

            void update_camera_view(float d_yaw, float d_pitch);

            void update_film_resolution(uint2 res);
        };
    }
}