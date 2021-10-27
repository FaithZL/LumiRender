//
// Created by Zero on 2021/4/8.
//


#include "spot_light.h"
#include "core/refl/factory.h"

namespace luminous {
    inline namespace render {

        SurfaceInteraction SpotLight::sample(LightLiSample *lls, float2 u, const SceneData *scene_data) const {
            SurfaceInteraction ret;
            ret.pos = _pos;
            return ret;
        }

        float SpotLight::fall_off(float3 w_world) const {
            float cos_theta = dot(w_world, _axis);
            if (cos_theta > _cos_theta_i) {
                return 1;
            } else if (cos_theta <= _cos_theta_o) {
                return 0;
            }
            float delta = (cos_theta - _cos_theta_o) / (_cos_theta_i - _cos_theta_o);
            return sqr(sqr(delta));
        }

        void SpotLight::print() const {
            printf("type:SpotLight,L:(%f,%f,%f)\n", _intensity.x, _intensity.y, _intensity.z);
        }

        LightLiSample SpotLight::Li(LightLiSample lls, const SceneData *data) const {
            float3 wi = lls.ctx.pos - lls.p_light.pos;
            float3 L = _intensity / length_squared(wi);
            wi = normalize(wi);
            float f = fall_off(wi);
            lls.L = L * f;
            lls.PDF_dir = 0;
            lls.wi = wi;
            return lls;
        }

        float SpotLight::PDF_Li(const LightSampleContext &ctx, const SurfaceInteraction &p_light,
                                float3 wi, const SceneData *data) const {
            return 0;
        }

        Spectrum SpotLight::power() const {
            return _intensity * 2.f * Pi * (1 - 0.5f * (_cos_theta_i + _cos_theta_o));
        }

    } // luminous::render
} // luminous