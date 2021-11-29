//
// Created by Zero on 2021/1/29.
//


#pragma once

#include "base.h"
#include "base_libs/optics/rgb.h"
#include "microfacet.h"

namespace luminous {
    inline namespace render {
        class DielectricBxDF {
        private:
            float4 Kr;
            float4 Kt;
            float _eta{0.f};
            MicrofacetDistribution _distribution{};
        public:
            LM_XPU DielectricBxDF() = default;

            LM_ND_XPU float4 base_color() const {
                return Kr;
            }

            LM_XPU DielectricBxDF(float4 kr, float4 kt, float eta, MicrofacetDistribution distribution)
                    : Kr(kr), Kt(kt), _eta(eta), _distribution(distribution) {}

            LM_ND_XPU Spectrum eval(float3 wo, float3 wi, TransportMode mode = TransportMode::Radiance) const;

            LM_ND_XPU float PDF(float3 wo, float3 wi, TransportMode mode = TransportMode::Radiance,
                                BxDFReflTransFlags sample_flags = BxDFReflTransFlags::All) const;

            LM_ND_XPU lstd::optional<BSDFSample> sample_f(float3 wo, float uc, float2 u,
                                                          TransportMode mode = TransportMode::Radiance,
                                                          BxDFReflTransFlags sample_flags = BxDFReflTransFlags::All) const;

            LM_ND_XPU BxDFFlags flags() const;
        };
    }
}