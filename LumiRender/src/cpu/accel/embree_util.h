//
// Created by Zero on 2021/8/10.
//


#pragma once

#include <embree3/rtcore.h>
#include "render/include/interaction.h"
#include "base_libs/geometry/util.h"

using std::cout;
using std::endl;

namespace luminous {
    inline namespace cpu {
        NDSC_INLINE RTCRay to_RTCRay(Ray r) {
            RTCRay ray{};
            ray.org_x = r.org_x;
            ray.org_y = r.org_y;
            ray.org_z = r.org_z;
            ray.dir_x = r.dir_x;
            ray.dir_y = r.dir_y;
            ray.dir_z = r.dir_z;
            ray.tnear = 0;
            ray.tfar  = r.t_max;
            ray.flags = 0;
            return ray;
        }

        NDSC_INLINE RTCRayHit to_RTCRayHit(Ray ray) {
            RTCRay rtc_ray = to_RTCRay(ray);
            RTCRayHit rh{};
            rh.ray = rtc_ray;
            rh.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            rh.hit.primID = RTC_INVALID_GEOMETRY_ID;
            return rh;
        }

        template<int ray_num>
        struct RTCType {
            using RayType = RTCRayNp;
            using HitType = RTCHitNp;
            using RHType = RTCRayHitNp;
        };

        template<>
        struct RTCType<4> {
            using RayType = RTCRay4;
            using HitType = RTCHit4;
            using RHType = RTCRayHit4;
        };

        template<>
        struct RTCType<8> {
            using RayType = RTCRay8;
            using HitType = RTCHit8;
            using RHType = RTCRayHit8;
        };

        template<>
        struct RTCType<16> {
            using RayType = RTCRay16;
            using HitType = RTCHit16;
            using RHType = RTCRayHit16;
        };

        template<int ray_num>
        NDSC_INLINE auto to_RTCRayN(Ray *ray) {
            typename RTCType<ray_num>::RayType ret{};
            parallel_for(ray_num, [&](uint idx, uint tid){
                ret.org_x[idx] = ray[idx].org_x;
                ret.org_y[idx] = ray[idx].org_y;
                ret.org_z[idx] = ray[idx].org_z;
                ret.dir_x[idx] = ray[idx].dir_x;
                ret.dir_y[idx] = ray[idx].dir_y;
                ret.dir_z[idx] = ray[idx].dir_z;
                ret.tnear[idx] = 0;
                ret.tfar[idx]  = ray[idx].t_max;
                ret.flags[idx] = 0;
            });
            return ret;
        }

        template<int ray_num>
        NDSC_INLINE auto to_RTCRayHitN(Ray *ray) {
            typename RTCType<ray_num>::RHType ret;
            ret.ray = to_RTCRayN<ray_num>(ray);
            return ret;
        }

        NDSC_INLINE RTCBounds to_RTCBounds(Box3f box) {
            return RTCBounds{box.lower.x,box.lower.y,box.lower.z,0,
                             box.upper.x,box.upper.y,box.upper.z,0};
        }

        NDSC_INLINE bool rtc_intersect(RTCScene scene, Ray ray, PerRayData *prd) {
            RTCIntersectContext context{};
            rtcInitIntersectContext(&context);
            RTCRayHit rh = to_RTCRayHit(ray);
            rtcIntersect1(scene, &context, &rh);
            prd->hit_point.instance_id = rh.hit.instID[0];
            prd->hit_point.triangle_id = rh.hit.primID;
            float w = 1 - rh.hit.u - rh.hit.v;
            prd->hit_point.bary = make_float2(w, rh.hit.u);
            return prd->is_hit();
        }

        NDSC_INLINE bool rtc_occlusion(RTCScene scene, Ray ray) {
            RTCIntersectContext context{};
            rtcInitIntersectContext(&context);
            RTCRay rtc_ray = to_RTCRay(ray);
            rtcOccluded1(scene, &context, &rtc_ray);
            return rtc_ray.tfar < 0;
        }
    }
}