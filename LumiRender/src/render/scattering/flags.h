//
// Created by Zero on 29/12/2021.
//


#pragma once

#include "base_libs/math/common.h"
#include "base_libs/sampling/warp.h"
#include "base_libs/geometry/common.h"
#include "base_libs/lstd/lstd.h"
#include "base_libs/optics/rgb.h"

namespace luminous {
    inline namespace render {
        // BxDFFlags Definition
        enum BxDFFlags : uint8_t {
            Unset = 0,
            Reflection = 1 << 0,
            Transmission = 1 << 1,
            Diffuse = 1 << 2,
            Glossy = 1 << 3,
            Specular = 1 << 4,
            // Composite _BxDFFlags_ definitions
            DiffRefl = Diffuse | Reflection,
            DiffTrans = Diffuse | Transmission,
            GlossyRefl = Glossy | Reflection,
            GlossyTrans = Glossy | Transmission,
            SpecRefl = Specular | Reflection,
            SpecTrans = Specular | Transmission,
            All = Diffuse | Glossy | Specular | Reflection | Transmission
        };

        enum class TransportMode : uint8_t {
            Radiance,
            Importance
        };

        template<typename T>
        LM_ND_XPU T cal_factor(TransportMode mode, T eta) {
            return mode == TransportMode::Radiance ? rcp(sqr(eta)) : T(1.f);
        }

        ND_XPU_INLINE constexpr BxDFFlags operator|(BxDFFlags a, BxDFFlags b) {
            return BxDFFlags((int) a | (int) b);
        }

        ND_XPU_INLINE int operator&(BxDFFlags a, BxDFFlags b) {
            return ((int) a & (int) b);
        }

        ND_XPU_INLINE BxDFFlags &operator|=(BxDFFlags &a, BxDFFlags b) {
            (int &) a |= int(b);
            return a;
        }

        ND_XPU_INLINE bool is_reflective(BxDFFlags f) {
            return f & BxDFFlags::Reflection;
        }

        ND_XPU_INLINE bool is_transmissive(BxDFFlags f) {
            return f & BxDFFlags::Transmission;
        }

        ND_XPU_INLINE bool is_diffuse(BxDFFlags f) {
            return f & BxDFFlags::Diffuse;
        }

        ND_XPU_INLINE bool is_glossy(BxDFFlags f) {
            return f & BxDFFlags::Glossy;
        }

        ND_XPU_INLINE bool is_specular(BxDFFlags f) {
            return f & BxDFFlags::Specular;
        }

        ND_XPU_INLINE bool is_non_specular(BxDFFlags f) {
            return (f & (BxDFFlags::Diffuse | BxDFFlags::Glossy));
        }
    }
}