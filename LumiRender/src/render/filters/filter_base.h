//
// Created by Zero on 2021/7/26.
//


#pragma once

#include "base_libs/math/common.h"

namespace luminous {
    inline namespace render {
        struct FilterBase {
        protected:
            const float2 _radius;
        public:
            FilterBase(const float2 r) : _radius(r){}
            LM_ND_XPU float2 radius() {
                return _radius;
            }
        }
    }
}