//
// Created by Zero on 2021/4/18.
//


#pragma once

#include "cuda.h"
#include "graphics/math/common.h"
#include "texture_base.h"
#include "vector_types.h"

namespace luminous {
    inline namespace render {
        template<typename T>
        class ImageTexture : public TextureBase {
            static_assert(std::is_same_v<T, luminous::math::float4> || std::is_same_v<T, float>,
                          "T must be float4 or float!");
        private:
            PixelFormat _pixel_format;
            CUtexObject _handle{0};
        public:
            ImageTexture(CUtexObject handle, PixelFormat pixel_format)
                    : _handle(handle), _pixel_format(pixel_format) {}

            XPU T eval(const TextureEvalContext &tec) {
                if constexpr (std::is_same_v<T, luminous::math::float4>) {
                    ::float4 val = tex2D<::float4>(_handle, tec.uv[0], 1 - tec.uv[1]);
                    return make_float4(val.x, val.y, val.z, val.w);
                } else if constexpr (std::is_same_v<T, float>) {
                    return tex2D<T>(_handle, tec.uv[0], 1 - tec.uv[1]);
                } else {
                    return T();
                }
            }

            std::string to_string() const {
                LUMINOUS_TO_STRING("name: %s", type_name(this));
            }

            static ImageTexture<T> create(const TextureConfig <T> &config) {
                return ImageTexture<T>((CUtexObject) config.handle, config.pixel_format);
            }
        };
    }
}