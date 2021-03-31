//
// Created by Zero on 2021/3/17.
//


#pragma once

#include "render/include/interaction.h"
#include "render/films/shader_include.h"
#include "render/samplers/shader_include.h"
#include "render/sensors/shader_include.h"
#include "gpu/framework/optix_params.h"

struct RadiancePRD {
    // TODO: move some state directly into payload registers?
    int pad;
};

static GPU_INLINE void *unpackPointer(unsigned int i0, unsigned int i1) {
    const unsigned long long uptr = static_cast<unsigned long long>( i0 ) << 32 | i1;
    void *ptr = reinterpret_cast<void *>( uptr );
    return ptr;
}

static GPU_INLINE void packPointer(void *ptr, unsigned int &i0, unsigned int &i1) {
    const unsigned long long uptr = reinterpret_cast<unsigned long long>( ptr );
    i0 = uptr >> 32;
    i1 = uptr & 0x00000000ffffffff;
}

static GPU_INLINE void setPayloadOcclusion(bool occluded) {
    optixSetPayload_0(static_cast<unsigned int>( occluded ));
}

static GPU_INLINE void traceRadiance(OptixTraversableHandle handle,
                                              luminous::Ray ray, RadiancePRD *prd) {
    unsigned int u0, u1;
    packPointer(prd, u0, u1);
    float3 origin = make_float3(ray.org_x, ray.org_y, ray.org_z);
    float3 direction = make_float3(ray.dir_x, ray.dir_y, ray.dir_z);
    optixTrace(
            handle,
            origin,
            direction,
            ray.t_min,
            ray.t_max,
            0.0f,                // rayTime
            OptixVisibilityMask(1),
            OPTIX_RAY_FLAG_NONE,
            luminous::RayType::Radiance,        // SBT offset
            luminous::RayType::Count,           // SBT stride
            luminous::RayType::Radiance,        // missSBTIndex
            u0, u1);
}

static GPU_INLINE bool traceOcclusion(OptixTraversableHandle handle, luminous::Ray ray) {
    unsigned int occluded = 0u;
    float3 origin = make_float3(ray.org_x, ray.org_y, ray.org_z);
    float3 direction = make_float3(ray.dir_x, ray.dir_y, ray.dir_z);
    optixTrace(
            handle,
            origin,
            direction,
            ray.t_min,
            ray.t_max,
            0.0f,                    // rayTime
            OptixVisibilityMask(1),
            OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT,
            luminous::RayType::Occlusion,        // SBT offset
            luminous::RayType::Count,           // SBT stride
            luminous::RayType::Occlusion,        // missSBTIndex
            occluded);
    return occluded;
}
