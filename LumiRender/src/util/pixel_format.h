//
// Created by Zero on 2021/4/18.
//


#pragma once

#include "graphics/math/common.h"

namespace luminous {
    inline namespace utility {
        using namespace std;
        enum struct PixelFormat : uint32_t {
            R8U, RG8U, RGBA8U,
            R32F, RG32F, RGBA32F,
        };

        namespace detail {

            template<typename T>
            struct PixelFormatImpl {

                template<typename U>
                static constexpr auto always_false = false;

                static_assert(always_false<T>, "Unsupported type for pixel format.");
            };

#define MAKE_PIXEL_FORMAT_OF_TYPE(Type, f)                  \
        template<>                                          \
        struct PixelFormatImpl<Type> {                      \
            static constexpr auto format = PixelFormat::f;  \
            static constexpr auto pixel_size = sizeof(Type);\
        };                                                  \

            MAKE_PIXEL_FORMAT_OF_TYPE(uchar, R8U)
            MAKE_PIXEL_FORMAT_OF_TYPE(uchar2, RG8U)
            MAKE_PIXEL_FORMAT_OF_TYPE(uchar4, RGBA8U)
            MAKE_PIXEL_FORMAT_OF_TYPE(float, R32F)
            MAKE_PIXEL_FORMAT_OF_TYPE(float2, RG32F)
            MAKE_PIXEL_FORMAT_OF_TYPE(float4, RGBA32F)

#undef MAKE_PIXEL_FORMAT_OF_TYPE
        }

        template<typename T>
        struct HighPrecision {
            template<typename U>
            static constexpr auto always_false = false;

            static_assert(always_false<T>, "Unsupported type for pixel format.");
        };
#define MAKE_HIGH_PRECISION(Type, HighType)                                                \
            template<>                                                                     \
            struct HighPrecision<Type>{                                                    \
                using type = HighType;                                                     \
                static constexpr auto format = detail::PixelFormatImpl<HighType>::format;  \
                                                                                           \
            };
        MAKE_HIGH_PRECISION(uint8_t, float)
        MAKE_HIGH_PRECISION(float, float)
        MAKE_HIGH_PRECISION(uint16_t , float2)
        MAKE_HIGH_PRECISION(float2 , float2)
        MAKE_HIGH_PRECISION(uint32_t, float4)
        MAKE_HIGH_PRECISION(float4, float4)
#undef MAKE_HIGH_PRECISION

        template<typename T>
        using PixelFormatImpl = detail::PixelFormatImpl<T>;
    }
}