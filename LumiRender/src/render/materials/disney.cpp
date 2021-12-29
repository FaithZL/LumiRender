//
// Created by Zero on 28/12/2021.
//

#include "disney.h"
#include "render/scene/scene_data.h"


namespace luminous {
    inline namespace render {

        BSDFWrapper DisneyMaterial::get_BSDF(const MaterialEvalContext &ctx,
                                             const SceneData *scene_data) const {
            // todo merge texture
            float4 color = scene_data->get_texture(_color).eval(ctx);
            float metallic = scene_data->get_texture(_metallic).eval(ctx).x;
            float eta = scene_data->get_texture(_eta).eval(ctx).x;
            float spec_trans = scene_data->get_texture(_spec_trans).eval(ctx).x;
            float diffuse_weight = (1 - metallic) * (1 - spec_trans);
            float diff_trans = scene_data->get_texture(_diff_trans).eval(ctx).x / 2.f;
            float roughness = scene_data->get_texture(_roughness).eval(ctx).x;
            float lum = Spectrum{color}.Y();
            float4 color_tint = lum > 0 ? (color / lum) : make_float4(1.f);
            float sheen_weight = scene_data->get_texture(_sheen).eval(ctx).x;
            float sheen_tint = scene_data->get_texture(_sheen_tint).eval(ctx).x;
            float4 color_sheen_tint = sheen_weight > 0.f ?
                                      lerp(sheen_tint, make_float4(1.f), color_tint) :
                                      make_float4(0.f);

            if (_thin) {

            }

            return BSDFWrapper();
        }
    }
}