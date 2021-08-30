//
// Created by Zero on 2021/5/21.
//

#pragma once

#include "core/backend/task.h"
#include "core/backend/managed.h"
#include "cpu/cpu_impl.h"

namespace luminous {
    inline namespace cpu {

        class CPUTask : public Task {
        public:
            explicit CPUTask(Context *context)
                : Task(create_cpu_device(), context) {}

            void init(const Parser &parser) override;

            void render_cli() override;

            void update_device_buffer() override;

            NDSC FrameBufferType *get_frame_buffer() override;

            NDSC float4 *get_accumulate_buffer() override;
        };
    } // luminous::cpu
} // luminous