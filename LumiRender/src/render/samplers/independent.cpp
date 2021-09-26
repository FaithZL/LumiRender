//
// Created by Zero on 2021/1/29.
//

#include "base_libs/math/common.h"
#include "independent.h"
#include "core/refl/factory.h"

namespace luminous {
    inline namespace render {

        void LCGSampler::start_pixel_sample(uint2 pixel, int sample_index, int dimension) {
            _rng.init(pixel * uint(sample_index));
        }

        float LCGSampler::next_1d() {
            return _rng.next();
        }

        float2 LCGSampler::next_2d() {
            return make_float2(next_1d(), next_1d());
        }

        REGISTER(LCGSampler)

        void PCGSampler::start_pixel_sample(uint2 pixel, int sample_index, int dimension) {
            _rng.set_sequence((pixel.x + pixel.y * 65536) | (uint64_t(_seed) << 32));
            _rng.advance(sample_index * 65536 + dimension);
        }

        float PCGSampler::next_1d() {
            return _rng.uniform<float>();
        }

        float2 PCGSampler::next_2d() {
            return make_float2(next_1d(), next_1d());
        }

        REGISTER(PCGSampler)
    }
}