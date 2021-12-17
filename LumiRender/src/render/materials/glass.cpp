//
// Created by Zero on 17/12/2021.
//

#include "glass.h"
#include "render/scene/scene_data.h"
#include "core/refl/factory.h"

namespace luminous {
    inline namespace render {

        BSDFWrapper GlassMaterial::get_BSDF(const MaterialEvalContext &ctx, const SceneData *scene_data) const {
            const Texture& tex = scene_data->get_texture(_color_idx);
            const Texture& eta_tex = scene_data->get_texture(_eta_idx);
            float4 color = tex.eval(ctx);
            float eta = eta_tex.eval(ctx).x;
            GlassBSDF glass_bsdf = create_glass_bsdf(color, eta);
            return {ctx.ng, ctx.ns, ctx.dp_dus, BSDF{glass_bsdf}};
        }
    }
}