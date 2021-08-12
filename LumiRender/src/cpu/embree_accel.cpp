//
// Created by Zero on 2021/5/16.
//

#include "embree_accel.h"
#include "util/stats.h"

namespace luminous {
    inline namespace cpu {
        RTCDevice EmbreeAccel::_rtc_device = nullptr;

        void EmbreeAccel::init_device() {
            _rtc_device = rtcNewDevice(nullptr);
        }

        EmbreeAccel::EmbreeAccel() {
            init_device();
            _rtc_scene = rtcNewScene(_rtc_device);
            rtcSetSceneBuildQuality(_rtc_scene, RTC_BUILD_QUALITY_HIGH);
            rtcSetSceneFlags(_rtc_scene, RTC_SCENE_FLAG_CONTEXT_FILTER_FUNCTION);
        }

        EmbreeAccel::~EmbreeAccel() {
            rtcReleaseScene(_rtc_scene);
        }

        std::string EmbreeAccel::description() const {
            return string_printf("EmbreeAccel");
        }

        void EmbreeAccel::build_bvh(const Managed<float3> &positions,
                                    const Managed<TriangleHandle> &triangles,
                                    const Managed<MeshHandle> &meshes, const Managed<uint> &instance_list,
                                    const Managed<Transform> &transform_list, const Managed<uint> &inst_to_transform) {
            TASK_TAG("build embree bvh");
            std::vector<RTCGeometry> rtc_geometries;
            rtc_geometries.reserve(meshes.size());
            for (const auto &mesh : meshes) {
                RTCGeometry rtc_geometry = build_mesh(positions, triangles, mesh);
                rtc_geometries.push_back(rtc_geometry);
            }
            for (int i = 0; i < instance_list.size(); ++i) {
                uint mesh_idx = instance_list[i];
                RTCScene rtc_scene = rtcNewScene(rtc_device());
                RTCGeometry mesh_geometry = rtc_geometries[mesh_idx];
                rtcCommitGeometry(mesh_geometry);
                rtcAttachGeometry(rtc_scene, mesh_geometry);
                rtcCommitScene(rtc_scene);
                uint transform_idx = inst_to_transform[i];
                const Transform &transform = transform_list[transform_idx];
                RTCGeometry instance = rtcNewGeometry(rtc_device(), RTC_GEOMETRY_TYPE_INSTANCE);
                rtcSetGeometryInstancedScene(instance, rtc_scene);
                rtcSetGeometryTransform(instance, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR, transform.mat4x4_ptr());
                rtcCommitGeometry(instance);
                rtcAttachGeometry(_rtc_scene, instance);
                rtcReleaseGeometry(instance);
            }
            for (auto geometry : rtc_geometries) {
                rtcReleaseGeometry(geometry);
            }
            rtcCommitScene(_rtc_scene);
        }

        RTCGeometry EmbreeAccel::build_mesh(const Managed<float3> &positions,
                                            const Managed<TriangleHandle> &triangles,
                                            const MeshHandle &mesh) {
            RTCGeometry rtc_geometry = rtcNewGeometry(rtc_device(), RTC_GEOMETRY_TYPE_TRIANGLE);
            auto pos = positions.const_host_buffer_view(mesh.vertex_offset, mesh.vertex_count);
            rtcSetSharedGeometryBuffer(rtc_geometry, RTC_BUFFER_TYPE_VERTEX,
                                       0, RTC_FORMAT_FLOAT3,
                                       pos.cbegin(), 0, sizeof(float3), pos.size());
            auto tri_list = triangles.const_host_buffer_view(mesh.triangle_offset, mesh.triangle_count);
            rtcSetSharedGeometryBuffer(rtc_geometry, RTC_BUFFER_TYPE_INDEX,
                                       0, RTC_FORMAT_UINT3, tri_list.cbegin(),
                                       0, sizeof(TriangleHandle), tri_list.size());
            return rtc_geometry;
        }

    } // luminous::cpu
} // luminous