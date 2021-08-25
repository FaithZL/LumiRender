//
// Created by Zero on 23/08/2021.
//


#pragma once

#include <optix.h>

#include <utility>
#include "core/concepts.h"
#include "core/backend/buffer.h"
#include "optix_params.h"
#include "gpu/framework/cuda_impl.h"

namespace luminous {
    inline namespace gpu {

        class GPUScene;

        struct ProgramName {
            const char *raygen{};
            const char *closesthit_closest{};
            const char *closesthit_any{};
            const char *miss_closest{};
            const char *miss_any{};
        };

        struct ProgramGroupTable {
            OptixProgramGroup raygen_prog_group{nullptr};
            OptixProgramGroup radiance_miss_group{nullptr};
            OptixProgramGroup occlusion_miss_group{nullptr};
            OptixProgramGroup radiance_hit_group{nullptr};
            OptixProgramGroup occlusion_hit_group{nullptr};

            ProgramGroupTable() = default;

            static constexpr auto size() {
                return sizeof(ProgramGroupTable) / sizeof(OptixProgramGroup);
            }

            void clear() {
                optixProgramGroupDestroy(raygen_prog_group);
                optixProgramGroupDestroy(radiance_miss_group);
                optixProgramGroupDestroy(occlusion_miss_group);
                optixProgramGroupDestroy(radiance_hit_group);
                optixProgramGroupDestroy(occlusion_hit_group);
            }
        };

        struct DevicePtrTable {
            Buffer<RayGenRecord> rg_record{nullptr};
            Buffer<SceneRecord> miss_record{nullptr};
            Buffer<SceneRecord> hit_record{nullptr};

            void clear() {
                rg_record.clear();
                miss_record.clear();
                hit_record.clear();
            }
        };

        class ShaderWrapper : public Noncopyable {
        private:
            DevicePtrTable _device_ptr_table;
            OptixShaderBindingTable _sbt{};
            ProgramGroupTable _program_group_table{};
        public:
            ShaderWrapper() = default;

            ShaderWrapper(ShaderWrapper &&other) noexcept
                    : _device_ptr_table(std::move(other._device_ptr_table)),
                      _sbt(other._sbt),
                      _program_group_table(other._program_group_table) {}

            ShaderWrapper(OptixModule optix_module, OptixDeviceContext optix_device_context,
                          const GPUScene *gpu_scene, std::shared_ptr<Device> device,
                          const ProgramName &program_name);

            NDSC const OptixShaderBindingTable *sbt_ptr() const { return &_sbt; }

            NDSC std::vector<OptixProgramGroup> program_groups() const {
                return {_program_group_table.raygen_prog_group,
                        _program_group_table.radiance_miss_group,
                        _program_group_table.occlusion_miss_group,
                        _program_group_table.radiance_hit_group,
                        _program_group_table.occlusion_hit_group};
            };

            void clear() {
                _program_group_table.clear();
                _device_ptr_table.clear();
            }

            ~ShaderWrapper() = default;

            ProgramGroupTable create_program_groups(OptixModule optix_module,
                                                    OptixDeviceContext optix_device_context,
                                                    const ProgramName &program_name);

            void create_sbt(const GPUScene *gpu_scene, std::shared_ptr<Device> device);
        };
    }
}