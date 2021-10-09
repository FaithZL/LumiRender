//
// Created by Zero on 2021/4/9.
//

#include "common.h"
#include "light_sampler.h"
#include "render/include/creator.h"
#include "render/lights/light.h"

namespace luminous {
    inline namespace render {

        void LightSampler::set_lights(BufferView<const Light> lights) {
            LUMINOUS_VAR_PTR_DISPATCH(set_lights, lights);
        }

        void LightSampler::set_infinite_lights(BufferView<const Light> lights) {
            LUMINOUS_VAR_PTR_DISPATCH(set_infinite_lights, lights);
        }

        size_t LightSampler::light_num() const {
            LUMINOUS_VAR_PTR_DISPATCH(light_num);
        }

        SampledLight LightSampler::sample(float u) const {
            LUMINOUS_VAR_PTR_DISPATCH(sample, u);
        }

        CPU_ONLY(LM_NODISCARD std::string LightSampler::to_string() const {
            LUMINOUS_VAR_PTR_DISPATCH(to_string);
        })

        SampledLight LightSampler::sample(const LightSampleContext &ctx, float u) const {
            LUMINOUS_VAR_PTR_DISPATCH(sample, ctx, u);
        }

        Spectrum LightSampler::estimate_direct_lighting(const SurfaceInteraction &si, Sampler &sampler,
                                                        uint64_t traversable_handle,
                                                        const SceneData *scene_data,
                                                        NEEData *NEE_data) const {
            auto sampled_light = sample(si, sampler.next_1d());
            if (sampled_light.is_valid()) {
                return sampled_light.light->estimate_direct_lighting(si, sampler,
                                                                     traversable_handle, scene_data,
                                                                     NEE_data) / sampled_light.PMF;
            }
            return {0.f};
        }

        const Light &LightSampler::light_at(uint idx) const {
            LUMINOUS_VAR_PTR_DISPATCH(light_at, idx);
        }

        float LightSampler::PMF(const Light &light) const {
            LUMINOUS_VAR_PTR_DISPATCH(PMF, light);
        }

        BufferView<const Light> LightSampler::lights() const {
            LUMINOUS_VAR_PTR_DISPATCH(lights);
        }

        BufferView<const Light> LightSampler::infinite_lights() const {
            LUMINOUS_VAR_PTR_DISPATCH(infinite_lights);
        }

        float LightSampler::PMF(const LightSampleContext &ctx, const Light &light) const {
            LUMINOUS_VAR_PTR_DISPATCH(PMF, ctx, light);
        }

        CPU_ONLY(LightSampler LightSampler::create(const LightSamplerConfig &config) {
            return detail::create_ptr<LightSampler>(config);
        })

        Spectrum LightSampler::on_miss(Ray ray, const SceneData *scene_data, Spectrum throughput) const {
            Spectrum L{0.f};
            BufferView<const Light> lights = infinite_lights();
            for (auto &light : lights) {
                L += throughput * light.on_miss(ray, scene_data);
            }
            return L;
        }

    } // luminous::render
} // luminous