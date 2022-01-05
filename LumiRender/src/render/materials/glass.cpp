//
// Created by Zero on 17/12/2021.
//

#include "glass.h"
#include "render/scene/scene_data.h"
#include "core/refl/factory.h"

namespace luminous {
    inline namespace render {

        BSDFWrapper GlassMaterial::get_BSDF(const MaterialEvalContext &ctx, const SceneData *scene_data) const {
            float4 color = scene_data->get_texture(_color_idx).eval(ctx);
            float eta = scene_data->get_texture(_eta_idx).eval(ctx).x;
            float2 roughness = make_float2(scene_data->get_texture(_roughness_idx).eval(ctx));
            if (_remapping_roughness) {
                roughness.x = Microfacet::roughness_to_alpha(roughness.x);
                roughness.y = Microfacet::roughness_to_alpha(roughness.y);
            }
            if (max(roughness.x, roughness.y) < 0.001) {
                auto glass_bsdf = create_glass_bsdf(color, eta);
                return {ctx.ng, ctx.ns, ctx.dp_dus, BSDF{glass_bsdf}};
            }
            auto glass_bsdf = create_rough_glass_bsdf(color, eta, roughness.x, roughness.y);
            return {ctx.ng, ctx.ns, ctx.dp_dus, BSDF{glass_bsdf}};
        }
    }
}