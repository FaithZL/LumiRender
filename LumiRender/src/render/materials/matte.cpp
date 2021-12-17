//
// Created by Zero on 2021/5/13.
//

#include "render/scene/scene_data.h"
#include "matte.h"
#include "core/refl/factory.h"

namespace luminous {
    inline namespace render {

        BSDFWrapper MatteMaterial::get_BSDF(const MaterialEvalContext &ctx, const SceneData *scene_data) const {
            Texture tex = scene_data->textures[_color_idx];
            float4 color = tex.eval(ctx);
            DiffuseBSDF diffuse_bsdf = create_diffuse_bsdf(color);
            BSDF bsdf{diffuse_bsdf};
            return {ctx.ng, ctx.ns, ctx.dp_dus, bsdf};
        }
    }
}