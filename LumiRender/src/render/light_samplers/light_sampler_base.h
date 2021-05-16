//
// Created by Zero on 2021/4/9.
//


#pragma once

#include "graphics/math/common.h"
#include "render/lights/light.h"
#include "core/backend/buffer_view.h"

namespace luminous {
    inline namespace render {
        struct SampledLight {
            const Light *light;
            float PMF{0.f};

            XPU SampledLight() = default;

            XPU SampledLight(const Light * light, float PMF)
                    : light(light), PMF(PMF) {}

            NDSC std::string to_string() const {
                return string_printf("sampled light :{PMF:%s, light:%s}",
                                     PMF, light->to_string().c_str());
            }
        };

        class LightSamplerBase {
        protected:
            BufferView<const Light> _lights;
        public:
            NDSC_XPU BufferView<const Light> lights() const {
                return _lights;
            }

            NDSC_XPU const Light& light_at(uint idx) const {
                return _lights[idx];
            }

            XPU void set_lights(BufferView<const Light> lights) {
                _lights = lights;
            }

            NDSC_XPU size_t light_num() const {
                return _lights.size();
            }
        };
    }
}