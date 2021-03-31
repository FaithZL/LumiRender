//
// Created by Zero on 2021/2/14.
//

#define GLOBAL extern "C" __global__ void

#include "optix_kernels.h"


extern "C" {
__constant__ luminous::LaunchParams params;
}

GLOBAL __raygen__rg() {
    const uint3 idx = optixGetLaunchIndex();
    auto pFilm = luminous::make_float2(idx.x, idx.y);
    auto camera = params.camera;
    auto film = camera->film();
    luminous::float3 o = luminous::make_float3(0.0,0.6,-1);
    luminous::float3 d = luminous::make_float3(0,0,3);
    auto b = traceOcclusion(params.traversable_handle,luminous::Ray(o,d));
    printf("%d\n", b);
//    float3 o = make_float3(0,0.45,-1);
//    float3 d = make_float3(0,0,3);

//    auto occ = traceOcclusion(
//            params.traversable_handle,
//            o,
//            d,
//            0.01f,  // tmin       // TODO: smarter offset
//            100  // tmax
//    );
//    RadiancePRD prd;
//    traceRadiance(params.traversable_handle, o, d, 0, 100, &prd);
}

GLOBAL __miss__radiance() {
//    printf("miss radiance\n");
}

GLOBAL __miss__shadow() {
//    printf("miss\n");
}

GLOBAL __closesthit__radiance() {
//    printf("__closesthit__radiance\n");

}

GLOBAL __closesthit__occlusion() {
//    printf("asdf\n");
    setPayloadOcclusion( true );
}
