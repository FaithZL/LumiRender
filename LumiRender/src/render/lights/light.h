//
// Created by Zero on 2021/4/7.
//


#pragma once

#include "render/samplers/sampler.h"
#include "base_libs/lstd/lstd.h"
#include "light_util.h"
#include "render/include/config.h"
#include "render/bxdfs/bsdf.h"

namespace luminous {
    inline namespace render {

        using lstd::Variant;

        class PointLight;

        class AreaLight;

        class Envmap;

        class SpotLight;

        class Light : BASE_CLASS(Variant<PointLight *, AreaLight *, SpotLight *, Envmap *>) {
        private:
            using BaseBinder::BaseBinder;
        public:
            REFL_CLASS(Light)

            GEN_BASE_NAME(Light)

            NDSC_XPU LightType type() const;

            CPU_ONLY(LM_NODISCARD std::string to_string() const;)

            NDSC_XPU bool is_delta() const;

            NDSC_XPU bool is_infinite() const;

            NDSC_XPU Spectrum on_miss(Ray ray, const SceneData *data) const;

            NDSC_XPU SurfaceInteraction sample(LightLiSample *lls, float2 u, const SceneData *scene_data) const;

            NDSC_XPU LightLiSample Li(LightLiSample lls, const SceneData *data) const;

            NDSC_XPU lstd::optional<LightLiSample> sample_Li(float2 u, LightLiSample lls, uint64_t traversable_handle,
                                                             const SceneData *scene_data) const;

            NDSC_XPU Spectrum MIS_sample_light(const SurfaceInteraction &si,
                                               Sampler &sampler, uint64_t traversable_handle,
                                               const SceneData *scene_data) const;

            NDSC_XPU Spectrum MIS_sample_BSDF(const SurfaceInteraction &si,
                                              Sampler &sampler, uint64_t traversable_handle,
                                              NEEData *NEE_data, const SceneData *data) const;

            NDSC_XPU Spectrum estimate_direct_lighting(const SurfaceInteraction &si,
                                                       Sampler &sampler, uint64_t traversable_handle,
                                                       const SceneData *scene_data, NEEData *NEE_data) const;

            NDSC_XPU float PDF_Li(const Interaction &ref_p, const SurfaceInteraction &p_light,
                                  float3 wi, const SceneData *data) const;

            NDSC_XPU Spectrum power() const;

            XPU void print() const;

            CPU_ONLY(static Light create(const LightConfig &config);)
        };
    } // luminous::render
} // luminous