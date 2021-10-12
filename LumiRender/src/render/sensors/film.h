//
// Created by Zero on 11/10/2021.
//


#pragma once

#include "base_libs/math/common.h"
#include "base_libs/lstd/lstd.h"
#include "render/include/config.h"
#include "core/backend/buffer_view.h"

namespace luminous {
    inline namespace render {

        struct Pixel {
            float4 rgb{};
            float weight_sum{};
        };

        enum FBState {
            Render,
            Normal,
            Albedo
        };

        class Film : BASE_CLASS() {
        public:
            REFL_CLASS(Film)

        protected:
            uint2 _resolution;
            Box2f _screen_window;
            FBState _fb_state{Render};
            BufferView<float4> _normal_buffer_view;
            BufferView<float4> _albedo_buffer_view;
            BufferView<float4> _render_buffer_view;
            BufferView<FrameBufferType> _frame_buffer_view;

            LM_XPU void _update();

            LM_ND_XPU uint _pixel_index(uint2 pixel) const {
                return pixel.y * _resolution.x + pixel.x;
            }

        public:
            CPU_ONLY(explicit Film(const FilmConfig &config) : Film(config.resolution) {})

            Film() = default;

            explicit Film(uint2 res)
                    : _resolution(res) {
                _update();
            }

            template<typename ...Args>
            LM_XPU void add_render_sample(uint2 pixel, Args &&...args) {
                add_render_sample(_pixel_index(pixel), std::forward<Args>(args)...);
            }

            LM_XPU void add_render_sample(uint pixel_index, Spectrum color, float weight, uint frame_index = 0u);

            template<typename ...Args>
            LM_XPU void add_normal_sample(uint2 pixel, Args &&...args) {
                add_normal_sample(_pixel_index(pixel), std::forward<Args>(args)...);
            }

            LM_XPU void add_normal_sample(uint pixel_index, float3 normal, float weight, uint frame_index = 0u);

            template<typename ...Args>
            LM_XPU void add_albedo_sample(uint2 pixel, Args &&...args) {
                add_albedo_sample(_pixel_index(pixel), std::forward<Args>(args)...);
            }

            LM_XPU void add_albedo_sample(uint pixel_index, float3 albedo, float weight, uint frame_index = 0u);

            LM_XPU void fill_frame_buffer(uint2 pixel) {
                fill_frame_buffer(_pixel_index(pixel));
            }

            LM_XPU void fill_frame_buffer(uint pixel_index);

            LM_XPU void set_resolution(uint2 resolution) {
                _resolution = resolution;
                _update();
            }

            LM_XPU void set_normal_buffer_view(BufferView<float4> buffer_view) {
                _normal_buffer_view = buffer_view;
            }

            LM_XPU void set_albedo_buffer_view(BufferView<float4> buffer_view) {
                _albedo_buffer_view = buffer_view;
            }

            LM_XPU void set_render_buffer_view(BufferView<float4> buffer_view) {
                _render_buffer_view = buffer_view;
            }

            LM_XPU void set_frame_buffer_view(BufferView<FrameBufferType> buffer_view) {
                _frame_buffer_view = buffer_view;
            }

            LM_ND_XPU uint2 resolution() const { return _resolution; }

            LM_ND_XPU Box2f screen_window() const { return _screen_window; }

            GEN_STRING_FUNC({
                                return string_printf("resolution : %s, pixel bounds : %s",
                                                     _resolution.to_string().c_str(),
                                                     _screen_window.to_string().c_str());
                            })
        };
    }
}