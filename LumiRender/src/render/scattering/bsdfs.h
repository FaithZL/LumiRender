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

        using DiffuseBSDF = BSDF_Ty<BSDFCommonData, FresnelNoOp, MicrofacetNone, DiffuseReflection>;

        ND_XPU_INLINE DiffuseBSDF create_diffuse_bsdf(float4 color) {
            BSDFCommonData data{color};
            return DiffuseBSDF(data, FresnelNoOp{}, MicrofacetNone{}, DiffuseReflection{});
        }

        using OrenNayarBSDF = BSDF_Ty<OrenNayarData, FresnelNoOp, MicrofacetNone, OrenNayar>;

        ND_XPU_INLINE OrenNayarBSDF create_oren_nayar_bsdf(float4 color, float sigma) {
            OrenNayarData data{color, sigma};
            return OrenNayarBSDF(data, FresnelNoOp{}, MicrofacetNone{}, OrenNayar{});
        }

        using MirrorBSDF = BSDF_Ty<BSDFCommonData, FresnelNoOp, MicrofacetNone, SpecularReflection>;

        ND_XPU_INLINE MirrorBSDF create_mirror_bsdf(float4 color) {
            BSDFCommonData data{color};
            return MirrorBSDF(data, FresnelNoOp{}, MicrofacetNone{}, SpecularReflection{});
        }

        using GlassBSDF = BSDF_Ty<BSDFCommonData, FresnelDielectric, MicrofacetNone, SpecularFresnel>;

        ND_XPU_INLINE GlassBSDF create_glass_bsdf(float4 color, float eta) {
            BSDFCommonData data{color};
            return GlassBSDF(data, FresnelDielectric{eta}, MicrofacetNone{}, SpecularFresnel{});
        }

        class BSDF : public Variant<DiffuseBSDF, OrenNayarBSDF, MirrorBSDF, GlassBSDF> {
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