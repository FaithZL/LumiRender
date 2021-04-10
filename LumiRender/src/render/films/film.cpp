//
// Created by Zero on 2021/3/5.
//

#include "film.h"
#include "render/include/creator.h"

namespace luminous {
    inline namespace render {
        const char *Film::name() {
            LUMINOUS_VAR_DISPATCH(name);
        }

        uint2 Film::resolution() const {
            LUMINOUS_VAR_DISPATCH(resolution);
        }

        Box2f Film::screen_window() const {
            LUMINOUS_VAR_DISPATCH(screen_window);
        }

        void Film::set_resolution(uint2 res) {
            LUMINOUS_VAR_DISPATCH(set_resolution, res);
        }

        void Film::add_sample(uint2 pixel, float3 color, float weight, uint frame_index) {
            LUMINOUS_VAR_DISPATCH(add_sample, pixel, color, weight, frame_index);
        }

        void Film::set_accumulate_buffer_view(BufferView<float4> buffer_view) {
            LUMINOUS_VAR_DISPATCH(set_accumulate_buffer_view, buffer_view);
        }

        void Film::set_frame_buffer_view(BufferView<FrameBufferType> buffer_view) {
            LUMINOUS_VAR_DISPATCH(set_frame_buffer_view, buffer_view);
        }

        std::string Film::to_string() const {
            LUMINOUS_VAR_DISPATCH(to_string);
        }

        Film Film::create(const FilmConfig &config) {
            return detail::create<Film>(config);
        }
    }
}