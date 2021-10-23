//
// Created by Zero on 14/10/2021.
//

#include "kernels.h"
#include "render/samplers/shader_include.h"

#ifdef __CUDACC__
#define GLOBAL_PREFIX extern "C" __constant__
#else
#define GLOBAL_PREFIX static
#endif

namespace luminous {
    inline namespace render {


        GLOBAL_PREFIX RTParam *rt_param;

        CPU_ONLY(void set_rt_param(RTParam *param) {
            rt_param = param;
        })

        void generate_primary_ray(int task_id, int n_item, int y0, int sample_index,
                                  RayQueue *ray_queue, SOA<PixelSampleState> *pixel_sample_state) {
            uint res_x = rt_param->camera->resolution().x;
            uint2 pixel = make_uint2(task_id % res_x, y0 + task_id / res_x);
            pixel_sample_state->pixel[task_id] = pixel;

            Sensor *camera = rt_param->camera;
            Sampler sampler = *(rt_param->sampler);
            sampler.start_pixel_sample(pixel, sample_index, 0);
            SensorSample ss = sampler.sensor_sample(pixel);
            auto[weight, ray] = camera->generate_ray(ss);
            pixel_sample_state->L[task_id] = {0.f};
            pixel_sample_state->filter_weight[task_id] = 1.f;

            ray_queue->push_primary_ray(ray, task_id);
        }

        void reset_ray_queue(int task_id, int n_item, RayQueue *ray_queue) {
            ray_queue->reset();
        }

        void reset_queues(int task_id, int n_item, RayQueue *ray_queue,
                          HitAreaLightQueue *hit_area_light_queue,
                          ShadowRayQueue *shadow_ray_queue,
                          EscapedRayQueue *escaped_ray_queue,
                          MaterialEvalQueue *material_eval_queue) {

        }

        void generate_ray_samples(int task_id, int n_item, const RayQueue *ray_queue,
                                  SOA<PixelSampleState> *pixel_sample_state) {

        }

        void process_escape_ray(int task_id, int n_item,
                                EscapedRayQueue *escaped_ray_queue) {

        }

        void process_emission(int task_id, int n_item,
                              HitAreaLightQueue *hit_area_light_queue) {

        }

        void eval_BSDFs(int task_id, int n_item,
                        MaterialEvalQueue *material_eval_queue) {

        }

    }
}