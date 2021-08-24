//
// Created by Zero on 2021/3/17.
//


#pragma once

#include <optix.h>
#include "base_libs/math/common.h"
#include "render/sensors/sensor.h"
#include "render/samplers/sampler.h"
#include "render/include/scene_data.h"

namespace luminous {
    inline namespace gpu {
        struct LaunchParams {
            OptixTraversableHandle traversable_handle;
            uint frame_index;
            uint max_depth;
            float rr_threshold;
            Sensor *camera;
            const Sampler *sampler;
        };
        
        template<typename T>
        struct Record {
            alignas(OPTIX_SBT_RECORD_ALIGNMENT) char header[OPTIX_SBT_RECORD_HEADER_SIZE];
            T data;
        };

        enum RayType {
            ClosestHit = 0,
            AnyHit = 1,
            Count
        };

        using RayGenRecord = Record<RayGenData>;
        using SceneRecord = Record<SceneData>;

        template<typename T>
        void mat4x4_to_array12(Matrix4x4<T> mat, T *output) {

            output[0] = mat[0][0];
            output[1] = mat[1][0];
            output[2] = mat[2][0];
            output[3] = mat[3][0];

            output[4] = mat[0][1];
            output[5] = mat[1][1];
            output[6] = mat[2][1];
            output[7] = mat[3][1];

            output[8] = mat[0][2];
            output[9] = mat[1][2];
            output[10] = mat[2][2];
            output[11] = mat[3][2];
        }
    }
}