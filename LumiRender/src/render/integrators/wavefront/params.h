//
// Created by Zero on 03/09/2021.
//


#pragma once


#include "gpu/accel/optix_defines.h"
#include "render/integrators/wavefront/work_queue.h"

namespace luminous {
    inline namespace render {
        struct WavefrontParams {
            const RayQueue *ray_queue;
            RayQueue *next_ray_queue;
            uint64_t traversable_handle;
            ShadowRayQueue *shadow_ray_queue;
            HitAreaLightQueue *hit_area_light_queue;
            EscapedRayQueue *escaped_ray_queue;
            MaterialEvalQueue *material_eval_queue;
            SOA <PixelSampleState> *pixel_sample_state;
        };
    }
}