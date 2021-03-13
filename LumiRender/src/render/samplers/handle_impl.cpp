//
// Created by Zero on 2021/2/25.
//

#include <render/include/sampler.h>

#include "lcg.h"
#include "pcg.h"

namespace luminous {

    inline namespace render {
        int SamplerHandle::spp() const {
            LUMINOUS_VAR_PTR_DISPATCH(spp)
        }

        void SamplerHandle::start_pixel_sample(uint2 pixel, int sample_index, int dimension) {
            LUMINOUS_VAR_PTR_DISPATCH(start_pixel_sample, pixel, sample_index, dimension)
        }

        const char *SamplerHandle::name() {
            LUMINOUS_VAR_PTR_DISPATCH(name)
        }

        float SamplerHandle::next_1d() {
            LUMINOUS_VAR_PTR_DISPATCH(next_1d)
        }

        float2 SamplerHandle::next_2d() {
            LUMINOUS_VAR_PTR_DISPATCH(next_2d)
        }

        std::string SamplerHandle::to_string() {
            LUMINOUS_VAR_PTR_DISPATCH(to_string)
        }

        SamplerHandle::~SamplerHandle() {
            //todo 之后考虑是否需要实现析构函数
        }

        namespace detail {
            template<uint8_t current_index>
            NDSC SamplerHandle create_sampler(const SamplerConfig &config, Allocator &alloc) {
                using Sampler = std::remove_pointer_t<std::tuple_element_t<current_index, SamplerHandle::TypeTuple>>;
                if (Sampler::name() == config.type) {
                    return SamplerHandle(Sampler::create(config, alloc));
                }
                return create_sampler<current_index + 1>(config, alloc);
            }

            template<>
            NDSC SamplerHandle
            create_sampler<std::tuple_size_v<SamplerHandle::TypeTuple>>(const SamplerConfig &config,
                                                                        Allocator &alloc) {
                LUMINOUS_ERROR("unknown sampler type:", config.type);
            }
        }

        SamplerHandle SamplerHandle::create(const SamplerConfig &config, Allocator alloc) {
            return detail::create_sampler<0>(config, alloc);
        }
    }
}