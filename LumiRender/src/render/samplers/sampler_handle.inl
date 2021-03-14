//
// Created by Zero on 2021/2/25.
//


#pragma once

#include "sampler_handle.h"

namespace luminous {

    inline namespace render {
        int SamplerHandle::spp() const {
            LUMINOUS_VAR_DISPATCH(spp)
        }

        void SamplerHandle::start_pixel_sample(uint2 pixel, int sample_index, int dimension) {
            LUMINOUS_VAR_DISPATCH(start_pixel_sample, pixel, sample_index, dimension)
        }

        const char *SamplerHandle::name() {
            LUMINOUS_VAR_DISPATCH(name)
        }

        float SamplerHandle::next_1d() {
            LUMINOUS_VAR_DISPATCH(next_1d)
        }

        float2 SamplerHandle::next_2d() {
            LUMINOUS_VAR_DISPATCH(next_2d)
        }

        std::string SamplerHandle::to_string() {
            LUMINOUS_VAR_DISPATCH(to_string)
        }

        namespace detail {
            template<uint8_t current_index>
            NDSC SamplerHandle create_sampler(const SamplerConfig &config) {
                using Sampler = std::remove_pointer_t<std::tuple_element_t<current_index, SamplerHandle::TypeTuple>>;
                if (Sampler::name() == config.type) {
                    return SamplerHandle(Sampler::create(config));
                }
                return create_sampler<current_index + 1>(config);
            }

            template<>
            NDSC SamplerHandle
            create_sampler<std::tuple_size_v<SamplerHandle::TypeTuple>>(const SamplerConfig &config) {
                LUMINOUS_ERROR("unknown sampler type:", config.type);
            }
        }

        SamplerHandle SamplerHandle::create(const SamplerConfig &config) {
            return detail::create_sampler<0>(config);
        }
    }
}