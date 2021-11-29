//
// Created by Zero on 2021/4/30.
//


#pragma once


#include "base_libs/lstd/variant.h"
#include "core/refl/reflection.h"
#include "render/include/interaction.h"
#include "render/include/config.h"

namespace luminous {
    inline namespace render {
        class MatteMaterial;

        class AssimpMaterial;

        class DielectricMaterial;

        using lstd::Variant;

        class Material : BASE_CLASS(Variant<MatteMaterial *, AssimpMaterial *, DielectricMaterial *>) {
        public:
            REFL_CLASS(Material)

            using BaseBinder::BaseBinder;
        public:
            GEN_BASE_NAME(Material)

            LM_ND_XPU BSDF get_BSDF(const MaterialEvalContext &ctx, const SceneData *scene_data) const;

            CPU_ONLY(LM_NODISCARD static std::pair<Material, std::vector<size_t>> create(const MaterialConfig &mc);)
        };

    } // luminous::render
} // luminous

