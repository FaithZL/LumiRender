//
// Created by Zero on 17/12/2021.
//


#pragma once

#include "render/textures/texture.h"
#include "render/scattering/bsdf_wrapper.h"
#include "parser/config.h"
#include "core/concepts.h"

namespace luminous {
    inline namespace render {
        class GlassMaterial : BASE_CLASS() {
        public:
            REFL_CLASS(GlassMaterial)

        private:
            index_t _color_idx{};
            index_t _eta_idx{};
            index_t _roughness_idx{};
            bool _remapping_roughness{};
        public:
            explicit GlassMaterial(index_t color_idx, index_t eta_idx, index_t roughness_idx, bool remapping_roughness)
                    : _color_idx(color_idx), _eta_idx(eta_idx),
                      _roughness_idx(roughness_idx),
                      _remapping_roughness(remapping_roughness) {}

            LM_ND_XPU BSDFWrapper get_BSDF(const MaterialEvalContext &ctx, const SceneData *scene_data) const;

            CPU_ONLY(explicit GlassMaterial(const MaterialConfig &mc)
                    : GlassMaterial(mc.color_tex.tex_idx(),
                                    mc.eta_tex.tex_idx(),
                                    mc.roughness_tex.tex_idx(),
                                    mc.remapping_roughness) {})
        };
    }
}