//
// Created by Zero on 2021/2/18.
//


#pragma once

#include "task.h"
#include "gpu/framework/cuda_impl.h"
#include "render/films/film.h"
#include "core/backend/managed.h"

namespace luminous {
    inline namespace gpu {
        class CUDATask : public Task {
        public:
            explicit CUDATask(Context *context)
                : Task(create_cuda_device(), context) {}

            void init(const Parser &parser) override;

            void update_device_buffer() override;

            void render_cli() override {}

            NDSC FrameBufferType *get_frame_buffer() override;

            NDSC float4 *get_accumulate_buffer() override;
        };
    }
}