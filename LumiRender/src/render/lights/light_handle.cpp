//
// Created by Zero on 2021/4/7.
//

#include "light_handle.h"

namespace luminous {
    inline namespace render {

        LightType LightHandle::type() const {
            LUMINOUS_VAR_DISPATCH(type);
        }

        Interaction LightHandle::sample(float u) const {
            LUMINOUS_VAR_DISPATCH(sample, u);
        }

        LightLiSample LightHandle::Li(LightLiSample lls) const {
            LUMINOUS_VAR_DISPATCH(Li, lls);
        }

        bool LightHandle::is_delta() const {
            LUMINOUS_VAR_DISPATCH(is_delta);
        }

        float LightHandle::PDF_Li(const Interaction &ref_p, float3 wi) const {
            LUMINOUS_VAR_DISPATCH(PDF_Li, ref_p, wi);
        }

        float3 LightHandle::power() const {
            LUMINOUS_VAR_DISPATCH(power);
        }

        std::string LightHandle::to_string() const {
            LUMINOUS_VAR_DISPATCH(to_string);
        }


    } // luminous::render
} // luminous