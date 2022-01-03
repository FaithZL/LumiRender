//
// Created by Zero on 14/12/2021.
//


#pragma once

#include "base.h"

namespace luminous {
    inline namespace render {

        class SpecularReflection : public BxDF<SpecularReflection> {
        public:
            using BxDF::BxDF;

            LM_ND_XPU static Spectrum safe_eval(float3 wo, float3 wi, BSDFHelper data,
                                                TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            LM_ND_XPU static float safe_PDF(float3 wo, float3 wi,
                                            BSDFHelper data,
                                            TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            LM_ND_XPU static BSDFSample _sample_f(float3 wo, float uc, float2 u,
                                                  Spectrum Fr, BSDFHelper data,
                                                  TransportMode mode = TransportMode::Radiance);

            LM_ND_XPU static BSDFSample sample_f(float3 wo, float uc, float2 u, BSDFHelper data,
                                                 TransportMode mode = TransportMode::Radiance);

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::SpecRefl;
            }

        };

        class SpecularTransmission : public BxDF<SpecularTransmission> {
        public:
            using BxDF::BxDF;

            LM_ND_XPU static Spectrum safe_eval(float3 wo, float3 wi, BSDFHelper data,
                                                TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            LM_ND_XPU static float safe_PDF(float3 wo, float3 wi,
                                            BSDFHelper data,
                                            TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            LM_ND_XPU static BSDFSample _sample_f(float3 wo, float uc, float2 u,
                                                  Spectrum Fr, BSDFHelper data,
                                                  TransportMode mode = TransportMode::Radiance);

            LM_ND_XPU static BSDFSample sample_f(float3 wo, float uc, float2 u, BSDFHelper data,
                                                 TransportMode mode = TransportMode::Radiance);

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::SpecTrans;
            }

        };
    }
}