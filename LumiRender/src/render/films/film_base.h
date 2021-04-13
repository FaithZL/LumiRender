//
// Created by Zero on 2021/3/5.
//


#pragma once

#include "graphics/math/common.h"
#include "graphics/lstd/lstd.h"
#include "render//include/config.h"
#include "core/backend/buffer_view.h"

namespace luminous {
    inline namespace render {
        struct Pixel {
            float4 rgb;
            float weight_sum;
        };

        using FrameBufferType = uint32_t;

        class FilmBase {
        protected:
            uint2 _resolution;
            Box2f _screen_window;
//            float4 *_d_accumulate_buffer{nullptr};
//            FrameBufferType *_d_frame_buffer{nullptr};

            BufferView<float4> _accumulate_buffer_view;
            BufferView<FrameBufferType> _frame_buffer_view;

            XPU void update() {
                auto aspect = float(_resolution.x) / float(_resolution.y);
                if (aspect > 1.f) {
                    _screen_window.lower = make_float2(-aspect, -1.f);
                    _screen_window.upper = make_float2(aspect, 1.f);
                } else {
                    _screen_window.lower = make_float2(-1.f, -1.f / aspect);
                    _screen_window.upper = make_float2(1.f, 1.f / aspect);
                }
            }
        public:
            FilmBase(uint2 res)
                : _resolution(res) {
                update();
            }

            XPU void set_resolution(uint2 resolution) {
                _resolution = resolution;
                update();
            }

            void set_accumulate_buffer_view(BufferView<float4> buffer_view) {
                _accumulate_buffer_view = buffer_view;
            }

            void set_frame_buffer_view(BufferView<FrameBufferType> buffer_view) {
                _frame_buffer_view = buffer_view;
            }

            NDSC_XPU uint2 resolution() const { return _resolution; }

            NDSC_XPU Box2f screen_window() const { return _screen_window; }

            NDSC std::string _to_string() const {
                return string_printf("resolution : %s, pixel bounds : %s", 
                                    _resolution.to_string().c_str(),
                                    _screen_window.to_string().c_str());
            }
        };
    }
}