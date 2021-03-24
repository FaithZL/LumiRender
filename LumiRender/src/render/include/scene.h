//
// Created by Zero on 2021/3/24.
//


#pragma once

#include "core/concepts.h"
#include "graphics/math/common.h"
#include "shape.h"

namespace luminous {
    inline namespace render {
        using namespace std;
        class Scene : public Noncopyable {
        protected:
            vector<uint> _cpu_instance_to_mesh_idx{};
            vector<uint> _cpu_instance_to_transform_idx{};
            vector<float4x4> _cpu_transforms{};

            vector<MeshHandle> _cpu_meshes{};
            vector<float3> _cpu_positions{};
            vector<float3> _cpu_normals{};
            vector<float2> _cpu_tex_coords{};
            vector<TriangleHandle> _cpu_triangles{};
        public:

        };
    }
}