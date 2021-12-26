//
// Created by Zero on 16/12/2021.
//


#pragma once

#include "bsdf_ty.h"
#include "microfacet.h"
#include "bsdf_data.h"
#include "fresnel.h"
#include "diffuse_scatter.h"
#include "microfacet_scatter.h"
#include "specular_scatter.h"

namespace luminous {
    inline namespace render {

        using DiffuseBSDF = BSDF_Ty<DiffuseReflection>;

        ND_XPU_INLINE DiffuseBSDF create_diffuse_bsdf(float4 color) {
            return DiffuseBSDF(BSDFData::create_diffuse_data(color),
                               Fresnel{NoOp}, Microfacet{}, DiffuseReflection{});
        }

        using OrenNayarBSDF = BSDF_Ty<OrenNayar>;

        ND_XPU_INLINE OrenNayarBSDF create_oren_nayar_bsdf(float4 color, float sigma) {
            return OrenNayarBSDF(BSDFData::create_oren_nayar_data(color, sigma),
                                 Fresnel{NoOp}, Microfacet{}, OrenNayar{});
        }

        using MirrorBSDF = BSDF_Ty<SpecularReflection>;

        ND_XPU_INLINE MirrorBSDF create_mirror_bsdf(float4 color) {
            return MirrorBSDF(BSDFData::create_mirror_data(color),
                              Fresnel{NoOp}, Microfacet{}, SpecularReflection{});
        }

        using GlassBSDFOld = BSDF_Ty<SpecularFresnel>;

        ND_XPU_INLINE GlassBSDFOld create_glass_bsdf_old(float4 color, float eta) {
            return GlassBSDFOld(BSDFData::create_glass_data(color, eta), Fresnel{Dielectric}, Microfacet{},
                                SpecularFresnel{});
        }

        using GlassBSDF = FresnelBSDF<SpecularReflection, SpecularTransmission>;

        ND_XPU_INLINE GlassBSDF create_glass_bsdf(float4 color, float eta,
                                                  bool valid_refl = true, bool valid_trans = true) {
            return GlassBSDF(BSDFData::create_glass_data(color, eta), Fresnel{Dielectric}, Microfacet{},
                             SpecularReflection{valid_refl}, SpecularTransmission{valid_trans});
        }

        using RoughGlassBSDFOld = BSDF_Ty<MicrofacetFresnel>;

        ND_XPU_INLINE RoughGlassBSDFOld
        create_rough_glass_bsdf_old(float4 color, float eta, float alpha_x, float alpha_y) {
            return RoughGlassBSDFOld(BSDFData::create_glass_data(color, eta), Fresnel{Dielectric},
                                     Microfacet{alpha_x, alpha_y, GGX}, MicrofacetFresnel{});
        }

        using RoughGlassBSDF = FresnelBSDF<MicrofacetReflection, MicrofacetTransmission>;

        ND_XPU_INLINE RoughGlassBSDF
        create_rough_glass_bsdf(float4 color, float eta, float alpha_x, float alpha_y,
                                bool valid_refl = true, bool valid_trans = true) {
            return RoughGlassBSDF(BSDFData::create_glass_data(color, eta),
                                  Fresnel{Dielectric}, Microfacet{alpha_x, alpha_y},
                                  MicrofacetReflection{valid_refl},
                                  MicrofacetTransmission{valid_trans});
        }

        class BSDF : public Variant<DiffuseBSDF, OrenNayarBSDF, MirrorBSDF,
                GlassBSDF, RoughGlassBSDFOld, GlassBSDFOld, RoughGlassBSDF> {
        private:
            using Variant::Variant;
        public:
            GEN_BASE_NAME(BSDF)

            LM_ND_XPU Spectrum color() const {
                LUMINOUS_VAR_DISPATCH(color);
            }

            LM_ND_XPU Spectrum eval(float3 wo, float3 wi, BxDFFlags flags = BxDFFlags::All,
                                    TransportMode mode = TransportMode::Radiance) const {
                LUMINOUS_VAR_DISPATCH(eval, wo, wi, flags, mode);
            }

            LM_ND_XPU float PDF(float3 wo, float3 wi,
                                BxDFFlags flags = BxDFFlags::All,
                                TransportMode mode = TransportMode::Radiance) const {
                LUMINOUS_VAR_DISPATCH(PDF, wo, wi, flags, mode);
            }

            LM_ND_XPU BSDFSample sample_f(float3 wo, float uc, float2 u,
                                          BxDFFlags flags = BxDFFlags::All,
                                          TransportMode mode = TransportMode::Radiance) const {
                LUMINOUS_VAR_DISPATCH(sample_f, wo, uc, u, flags, mode);
            }

            LM_ND_XPU int match_num(BxDFFlags bxdf_flags) const {
                LUMINOUS_VAR_DISPATCH(match_num, bxdf_flags);
            }

            LM_ND_XPU BxDFFlags flags() const {
                LUMINOUS_VAR_DISPATCH(flags);
            }
        };
    }
}