//
// Created by Zero on 2021/4/29.
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
        enum BxDFFlags : uint8_t  {
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

        class BxDF {
        public:
            const bool valid{};

            LM_XPU explicit BxDF(bool valid = true) : valid(valid) {}
        };

#define GEN_MATCH_FLAGS_FUNC                            \
LM_ND_XPU bool match_flags(BxDFFlags bxdf_flags) {      \
    return (flags() & bxdf_flags) == flags() && valid;  \
}

        struct BSDFSample {
            Spectrum f_val{};
            float3 wi{};
            float PDF{-1.f};
            BxDFFlags flags{};
            float eta{1.f};

            LM_ND_XPU BSDFSample() = default;

            LM_ND_XPU BSDFSample(const Spectrum &val, float3 wi_, float PDF_, BxDFFlags flags_, float eta_ = 1)
                    : f_val(val), wi(wi_), PDF(PDF_), flags(flags_), eta(eta_) {
                CHECK_UNIT_VEC(wi_)
            }

            LM_ND_XPU bool valid() const {
                return PDF >= 0.f;
            }

            LM_ND_XPU bool is_non_specular() const {
                return luminous::is_non_specular(flags);
            }

            LM_ND_XPU bool is_reflective() const {
                return luminous::is_reflective(flags);
            }

            LM_ND_XPU bool is_transmissive() const {
                return luminous::is_transmissive(flags);
            }

            LM_ND_XPU bool is_diffuse() const {
                return luminous::is_diffuse(flags);
            }

            LM_ND_XPU bool is_glossy() const {
                return luminous::is_glossy(flags);
            }

            LM_ND_XPU bool is_specular() const {
                return luminous::is_specular(flags);
            }
        };
    }
}