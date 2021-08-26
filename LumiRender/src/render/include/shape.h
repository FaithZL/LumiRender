//
// Created by Zero on 2021/1/30.
//


#pragma once

#include "base_libs/geometry/common.h"
#include <vector>
#include "core/concepts.h"
#include "interaction.h"
#include "config.h"


namespace luminous {

    inline namespace render {
        using std::vector;
        using std::string;

        class Shape : public IObject {

        };

        struct Mesh : public Shape {
            Mesh(vector <float3> P,
                 vector <float3> N,
                 vector <float2> uv,
                 vector <TriangleHandle> T,
                 Box3f aabb,
                 index_t mat_idx = -1) :
                    positions(std::move(P)),
                    normals(std::move(N)),
                    tex_coords(std::move(uv)),
                    triangles(std::move(T)),
                    aabb(aabb),
                    mat_idx(mat_idx) {}

            vector <float3> normals;
            vector <float3> positions;
            vector <float2> tex_coords;
            vector <TriangleHandle> triangles;
            index_t mat_idx{invalid_uint32};
            Box3f aabb;
            mutable uint idx_in_meshes;

            NDSC bool has_material() const {
                return mat_idx != invalid_uint32;
            }
        };

        struct Model {
            explicit Model(const ShapeConfig &sc);

            Model() = default;

            NDSC string full_path(const string &fn) const { return fn.empty() ? fn : (directory / fn).string(); }

            NDSC bool has_custom_material() const {
                return !custom_material_name.empty();
            }

            string custom_material_name;
            std::filesystem::path directory;
            vector <std::shared_ptr<const Mesh>> meshes;
            vector <MaterialConfig> materials;
            vector <TextureConfig> textures;
            string key;
        };

        struct ModelInstance {
            ModelInstance(uint idx, const Transform &t, const char *n,
                          float3 emission = make_float3(0))
                    : model_idx(idx),
                      o2w(t),
                      name(n),
                      emission(emission) {}

            const char *name;
            const uint model_idx;
            const Transform o2w;
            float3 emission = make_float3(0.f);
        };
    }
}