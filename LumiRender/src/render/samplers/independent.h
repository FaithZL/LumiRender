//
// Created by Zero on 2021/1/29.
//


#pragma once

#include "graphics/math/rng.h"
#include "../include/config.h"
#include "sampler_base.h"

namespace luminous {
    inline namespace render {

        class LCGSampler : public SamplerBase {
        private:
            LCG<> _rng;
        public:
            XPU explicit LCGSampler(int spp = 1) : SamplerBase(spp) {}

            XPU void start_pixel_sample(uint2 pixel, int sample_index, int dimension);

            NDSC XPU float next_1d();

            NDSC XPU float2 next_2d();

            GEN_STRING_FUNC({
                LUMINOUS_TO_STRING("%s:{spp=%d}", type_name(this), spp())
            })

            static LCGSampler create(const SamplerConfig &config);
        };

        class PCGSampler : public SamplerBase {
        private:
            RNG _rng;
            int _seed;
        public:
            XPU explicit PCGSampler(int spp = 1) : SamplerBase(spp) {}

            XPU void start_pixel_sample(uint2 pixel, int sample_index, int dimension);

            NDSC XPU float next_1d();

            NDSC XPU float2 next_2d();

            GEN_STRING_FUNC({
                LUMINOUS_TO_STRING("%s:{spp=%d}", type_name(this), spp())
            })

            static PCGSampler create(const SamplerConfig &config);
        };
    }
}