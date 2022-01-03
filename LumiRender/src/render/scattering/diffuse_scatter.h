//
// Created by Zero on 15/12/2021.
//


#pragma once

#include "base.h"
#include "base_libs/optics/rgb.h"
#include "bsdf_data.h"
#include "microfacet.h"

namespace luminous {
    inline namespace render {
        class DiffuseReflection : public BxDF<DiffuseReflection> {
        public:
            using BxDF::BxDF;

            LM_ND_XPU Spectrum eval(float3 wo, float3 wi, BSDFHelper data,
                                    TransportMode mode = TransportMode::Radiance) const {
                return Spectrum{data.color() * constant::invPi};
            }

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::DiffRefl;
            }

        };

        class OrenNayar : public BxDF<OrenNayar> {
        public:
            using BxDF::BxDF;

            /**
             * fr(wi,wo) = R / PI * (A + B * max(0,cos(phi_i - phi_o)) * sin_alpha * tan_beta)
             * where A = 1 - sigma^2 / (2 * sigma^2 + 0.33)
             * 	     B = 0.45 * sigma^2 / (sigma^2 + 0.09)
             *   	 alpha = max(theta_i,theta_o)
             *		 beta = min(theta_i,theta_o)
             */
            LM_ND_XPU Spectrum eval(float3 wo, float3 wi, BSDFHelper helper,
                                    TransportMode mode = TransportMode::Radiance) const;

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::DiffRefl;
            }

        };

        class DiffuseTransmission : public BxDF<DiffuseTransmission> {
        public:
            using BxDF::BxDF;

            LM_ND_XPU Spectrum eval(float3 wo, float3 wi, BSDFHelper data,
                                    TransportMode mode = TransportMode::Radiance) const {
                return Spectrum{data.color() * constant::invPi};
            }

            LM_ND_XPU Spectrum safe_eval(float3 wo, float3 wi, BSDFHelper data,
                                         TransportMode mode = TransportMode::Radiance) const {
                return same_hemisphere(wo, wi) ? Spectrum{0.f} : eval(wo, wi, data);
            }

            LM_ND_XPU float safe_PDF(float3 wo, float3 wi,
                                     BSDFHelper helper,
                                     TransportMode mode = TransportMode::Radiance) const {
                return same_hemisphere(wo, wi) ? 0.f : cosine_hemisphere_PDF(Frame::abs_cos_theta(wi));
            }

            LM_ND_XPU BSDFSample sample_f(float3 wo, float uc, float2 u, BSDFHelper helper,
                                          TransportMode mode = TransportMode::Radiance) const {
                float3 wi = square_to_cosine_hemisphere(u);
                wi.z = wo.z > 0 ? -wi.z : wi.z;
                float PDF_val = safe_PDF(wo, wi, helper, mode);
                if (PDF_val == 0.f) {
                    return {};
                }
                Spectrum f = eval(wo, wi, helper, mode);
                return {f, wi, PDF_val, BxDFFlags::Reflection};
            }

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::DiffTrans;
            }
        };
    }
}