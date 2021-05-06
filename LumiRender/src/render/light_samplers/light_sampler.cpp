//
// Created by Zero on 2021/4/9.
//

#include "light_sampler.h"
#include "render/include/creator.h"

namespace luminous {
    inline namespace render {

        void LightSampler::set_lights(BufferView<const Light> lights) {
            LUMINOUS_VAR_DISPATCH(set_lights, lights);
        }

        size_t LightSampler::light_num() {
            LUMINOUS_VAR_DISPATCH(light_num);
        }

        lstd::optional<SampledLight> LightSampler::sample(float u) const {
            LUMINOUS_VAR_DISPATCH(sample, u);
        }

        lstd::optional<SampledLight> LightSampler::sample(const LightSampleContext &ctx, float u) const {
            LUMINOUS_VAR_DISPATCH(sample, ctx, u);
        }

        float3 LightSampler::estimate_direct_lighting(const LightSampleContext &ctx, const BSDF &bsdf, Sampler &sampler,
                                                      uint64_t traversable_handle, const HitGroupData *hit_group_data,
                                                      float3 *wi) const {
            return float3();
        }

        float LightSampler::PMF(const Light &light) const {
            LUMINOUS_VAR_DISPATCH(PMF, light);
        }

        BufferView<const Light> LightSampler::lights() const {
            LUMINOUS_VAR_DISPATCH(lights);
        }

        float LightSampler::PMF(const LightSampleContext &ctx, const Light &light) const {
            LUMINOUS_VAR_DISPATCH(PMF, ctx, light);
        }

        LightSampler LightSampler::create(const LightSamplerConfig &config) {
            return detail::create<LightSampler>(config);
        }

    } // luminous::render
} // luminous