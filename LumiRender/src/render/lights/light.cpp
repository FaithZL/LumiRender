//
// Created by Zero on 2021/4/7.
//

#include "light.h"
#include "render/include/creator.h"

namespace luminous {
    inline namespace render {

        LightType Light::type() const {
            LUMINOUS_VAR_DISPATCH(type);
        }

        Interaction Light::sample(float u, const HitGroupData * hit_group_data) const {
            LUMINOUS_VAR_DISPATCH(sample, u,hit_group_data);
        }

        LightLiSample Light::Li(LightLiSample lls) const {
            LUMINOUS_VAR_DISPATCH(Li, lls);
        }

        bool Light::is_delta() const {
            LUMINOUS_VAR_DISPATCH(is_delta);
        }

        float Light::PDF_Li(const Interaction &ref_p, const Interaction &p_light) const {
            LUMINOUS_VAR_DISPATCH(PDF_Li, ref_p, p_light);
        }

        float3 Light::power() const {
            LUMINOUS_VAR_DISPATCH(power);
        }

        Light Light::create(const LightConfig &config) {
            return detail::create<Light>(config);
        }
    } // luminous::render
} // luminous