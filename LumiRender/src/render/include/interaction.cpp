//
// Created by Zero on 2021/5/8.
//

#include "interaction.h"
#include "render/include/shader_data.h"
#include "render/lights/light.h"
#include "render/bxdfs/bsdf.h"
#include "render/materials/material.h"

namespace luminous {
    inline namespace render {
        Spectrum SurfaceInteraction::Le(float3 w) const {
            return has_emission() ? light->get<AreaLight>()->L(*this, w) : 0.f;
        }

        SurfaceInteraction PerRayData::compute_surface_interaction(Ray ray) const {
            auto si = scene_data()->compute_surface_interaction(closest_hit);
            si.init_BSDF(scene_data());
            si.wo = normalize(-ray.direction());
            return si;
        }

        lstd::optional<BSDF> SurfaceInteraction::get_BSDF(const SceneData *scene_data) const {
            if (!has_material()) {
                return {};
            }
            auto bsdf = material->get_BSDF(*this, scene_data);
            return lstd::optional<BSDF>(bsdf);
        }
    } // luminous::render
} // luminous