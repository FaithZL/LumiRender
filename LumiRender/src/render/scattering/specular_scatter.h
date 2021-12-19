//
// Created by Zero on 14/12/2021.
//


#pragma once

#include "base.h"
#include "fresnel.h"

namespace luminous {
    inline namespace render {

        class SpecularReflection : public BxDF {
        public:
            using BxDF::BxDF;

            template<typename TData, typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static Spectrum eval(float3 wo, float3 wi, TData data,
                                    TFresnel fresnel,
                                    TMicrofacet microfacet = {},
                                    TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            template<typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static float PDF(float3 wo, float3 wi,
                                TFresnel fresnel = {},
                                TMicrofacet microfacet = {},
                                TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            template<typename TData, typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static BSDFSample sample_f(float3 wo, float uc, float2 u, TData data,
                                          TFresnel fresnel,
                                          TMicrofacet microfacet = {},
                                          TransportMode mode = TransportMode::Radiance) {

                float3 wi = make_float3(-wo.x, -wo.y, wo.z);
                Spectrum val = fresnel.eval(Frame::cos_theta(wi)) * data.color / Frame::abs_cos_theta(wi);
                float PDF = 1.f;
                return {val, wi, PDF, BxDFFlags::SpecRefl, fresnel.eta};
            }

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::SpecRefl;
            }

            GEN_MATCH_FLAGS_FUNC
        };

        class SpecularTransmission : public BxDF {
        public:
            using BxDF::BxDF;

            template<typename TData, typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static Spectrum eval(float3 wo, float3 wi, TData data,
                                    TFresnel fresnel,
                                    TMicrofacet microfacet = {},
                                    TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            template<typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static float PDF(float3 wo, float3 wi,
                                TFresnel fresnel = {},
                                TMicrofacet microfacet = {},
                                TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            template<typename TData, typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static BSDFSample sample_f(float3 wo, float uc, float2 u, TData data,
                                          TFresnel fresnel,
                                          TMicrofacet microfacet = {},
                                          TransportMode mode = TransportMode::Radiance) {
                float3 wi{};
                using eta_type = decltype(TFresnel::eta);
                bool valid = refract(wo, make_float3(0, 0, 1), fresnel.eta, &fresnel.eta, &wi);
                if (!valid) {
                    return {};
                }
                auto Fr = fresnel.eval(Frame::cos_theta(wi));
                auto ft = (eta_type(1) - Fr) / Frame::abs_cos_theta(wi);
                eta_type factor = cal_factor(mode, fresnel.eta);
                Spectrum val = ft * data.color * factor;
                return {val, wi, 1, Transmission, fresnel.eta};
            }

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::SpecTrans;
            }

            GEN_MATCH_FLAGS_FUNC
        };

        class SpecularFresnel : public BxDF {
        public:
            using BxDF::BxDF;

            template<typename TData, typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static Spectrum eval(float3 wo, float3 wi, TData data,
                                           TFresnel fresnel,
                                           TMicrofacet microfacet = {},
                                           TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            template<typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static float PDF(float3 wo, float3 wi,
                                       TFresnel fresnel = {},
                                       TMicrofacet microfacet = {},
                                       TransportMode mode = TransportMode::Radiance) {
                return 0.f;
            }

            template<typename TData, typename TFresnel, typename TMicrofacet>
            LM_ND_XPU static BSDFSample sample_f(float3 wo, float uc, float2 u, TData data,
                                                 TFresnel fresnel,
                                                 TMicrofacet microfacet = {},
                                                 TransportMode mode = TransportMode::Radiance) {
                float Fr = fresnel.eval(Frame::cos_theta(wo));
                BSDFSample ret;
                if (uc < Fr) {
                    ret = SpecularReflection::sample_f(wo, uc, u, data, fresnel, microfacet, mode);
                    ret.PDF = Fr;
                } else {
                    ret = SpecularTransmission::sample_f(wo, uc, u, data, fresnel, microfacet, mode);
                    ret.PDF = 1 - Fr;
                }
                return ret;
            }

            LM_ND_XPU constexpr static BxDFFlags flags() {
                return BxDFFlags::SpecTrans | BxDFFlags::SpecRefl;
            }

            GEN_MATCH_FLAGS_FUNC
        };
    }
}