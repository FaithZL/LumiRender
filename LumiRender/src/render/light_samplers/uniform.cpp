//
// Created by Zero on 2021/4/9.
//

#include "uniform.h"
#include "render/include/creator.h"

namespace luminous {
    inline namespace render {

        lstd::optional<SampledLight> UniformLightSampler::sample(float u) const {
            if (_lights.empty()) {
                return {};
            }
            int lightIndex = std::min<int>(u * light_num(), light_num() - 1);
            return SampledLight(_lights[lightIndex], 1.f / light_num());
        }

        lstd::optional<SampledLight> UniformLightSampler::sample(const LightSampleContext &ctx, float u) const {
            return sample(u);
        }

        float UniformLightSampler::PMF(const Light &light) const {
            return light_num() == 0 ? 0 : 1.f / light_num();
        }

        float UniformLightSampler::PMF(const LightSampleContext &ctx, const Light &light) const {
            return PMF(light);
        }

        std::string UniformLightSampler::to_string() const {
            LUMINOUS_TO_STRING("light sampler : %s",type_name(this));
        }

        UniformLightSampler UniformLightSampler::create(const LightSamplerConfig &config) {
            return UniformLightSampler();
        }
    } // luminous::render
} // luminous