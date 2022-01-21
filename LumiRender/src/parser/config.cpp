//
// Created by Zero on 30/12/2021.
//

#include "config.h"
#include "core/logging.h"
#include "util/image.h"

namespace luminous {
    inline namespace render {

        Transform TransformConfig::create() const {
            if (type() == "matrix4x4") {
                return Transform(mat4x4);
            } else if (type() == "trs") {
                auto tt = Transform::translation(t);
                auto rr = Transform::rotation(make_float3(r), r.w);
                auto ss = Transform::scale(s);
                return tt * rr * ss;
            } else if (type() == "yaw_pitch") {
                auto yaw_t = Transform::rotation_y(yaw);
                auto pitch_t = Transform::rotation_x(-pitch);
                auto tt = Transform::translation(position);
                return tt * pitch_t * yaw_t;
            }
            LUMINOUS_ERROR("unknown transform type ", type());
        }

        void MaterialConfig::fill_tex_idx_by_name(vector<MaterialAttrConfig> &tex_configs, MaterialAttrConfig &tc,
                                                  bool force, bool check) {
            if (tc.name.empty()) {
                return;
            }
            auto idx = lstd::find_index_if(tex_configs, [&](const MaterialAttrConfig &tex_config) {
                return tex_config.name == tc.name;
            });
            tc.fill_tex_idx(idx, force);
            if (check) {
                DCHECK(tc.tex_valid())
            }
        }

        void MaterialConfig::fill_tex_configs(vector<MaterialAttrConfig> &tex_configs) {
            // common data
            if (!normal_tex.name.empty()) {
                fill_tex_idx_by_name(tex_configs, normal_tex, false, false);
            }

            if (type() == full_type("AssimpMaterial")) {
                int64_t index = lstd::find_index_if(tex_configs, [&](const MaterialAttrConfig &tex_config) {
                    return tex_config == color_tex;
                });
                if (index == -1) {
                    color_tex.set_tex_idx(tex_configs.size());
                    tex_configs.push_back(color_tex);
                } else {
                    color_tex.set_tex_idx(index);
                }

                index = lstd::find_index_if(tex_configs, [&](const MaterialAttrConfig &tex_config) {
                    return tex_config == specular_tex;
                });
                if (index == -1) {
                    specular_tex.fill_tex_idx(tex_configs.size());
                    tex_configs.push_back(specular_tex);
                } else {
                    specular_tex.fill_tex_idx(index);
                }

                index = lstd::find_index_if(tex_configs, [&](const MaterialAttrConfig &tex_config) {
                    return tex_config == normal_tex;
                });
                if (index == -1 && !normal_tex.name.empty()) {
                    normal_tex.fill_tex_idx(tex_configs.size());
                    tex_configs.push_back(normal_tex);
                } else {
                    normal_tex.fill_tex_idx(index);
                }

            } else if (type() == full_type("MatteMaterial")) {

                fill_tex_idx_by_name(tex_configs, color_tex);

            } else if (type() == full_type("MetalMaterial")) {

                fill_tex_idx_by_name(tex_configs, roughness_tex);

                fill_tex_idx_by_name(tex_configs, k_tex);

                fill_tex_idx_by_name(tex_configs, eta_tex);

            } else if (type() == full_type("GlassMaterial")) {
                fill_tex_idx_by_name(tex_configs, color_tex);

                fill_tex_idx_by_name(tex_configs, roughness_tex);

                fill_tex_idx_by_name(tex_configs, eta_tex);

            } else if (type() == full_type("FakeMetalMaterial")) {

                fill_tex_idx_by_name(tex_configs, color_tex);
                fill_tex_idx_by_name(tex_configs, roughness_tex);

            } else if (type() == full_type("DisneyMaterial")) {

                fill_tex_idx_by_name(tex_configs, color_tex);
                fill_tex_idx_by_name(tex_configs, roughness_tex);
                fill_tex_idx_by_name(tex_configs, eta_tex);
                fill_tex_idx_by_name(tex_configs, metallic_tex);
                fill_tex_idx_by_name(tex_configs, specular_tint_tex);
                fill_tex_idx_by_name(tex_configs, anisotropic_tex);
                fill_tex_idx_by_name(tex_configs, sheen_tex);
                fill_tex_idx_by_name(tex_configs, sheen_tint_tex);
                fill_tex_idx_by_name(tex_configs, clearcoat_tex);
                fill_tex_idx_by_name(tex_configs, clearcoat_gloss_tex);
                fill_tex_idx_by_name(tex_configs, spec_trans_tex);
                fill_tex_idx_by_name(tex_configs, scatter_distance_tex);
                fill_tex_idx_by_name(tex_configs, flatness_tex);
                fill_tex_idx_by_name(tex_configs, diff_trans_tex);

            }
        #if 0
        // Disable SubsurfaceMaterial parsing temporarily
        else if(type() == full_type("SubsurfaceMaterial")) {
            fill_tex_idx_by_name(tex_configs, sigma_s);
            fill_tex_idx_by_name(tex_configs, sigma_a);
            fill_tex_idx_by_name(tex_configs, reflectance);
            fill_tex_idx_by_name(tex_configs, mfp);
            fill_tex_idx_by_name(tex_configs, uroughness);
            fill_tex_idx_by_name(tex_configs, vroughness);
        }
        #endif
        }

        void LightConfig::fill_tex_config(vector<MaterialAttrConfig> &tex_configs) {
            if (type() != full_type("Envmap")) {
                return;
            }
            int idx = lstd::find_index_if(tex_configs, [&](const MaterialAttrConfig &tex_config) {
                return tex_config.name == texture_config.name;
            });
            texture_config = tex_configs[idx];
            texture_config.fill_tex_idx(idx);
        }
    }
}