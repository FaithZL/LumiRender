//
// Created by Zero on 2021/4/7.
//

#include "area_light.h"
#include "render/scene/scene_data.h"
#include "core/refl/factory.h"

namespace luminous {
    inline namespace render {

        AreaLight::AreaLight(uint inst_idx, float3 L, float area, bool two_sided)
                : BaseBinder<LightBase>(LightType::Area),
                  _inst_idx(inst_idx),
                  L(L),
                  _inv_area(1 / area),
                  _two_sided(two_sided) {}

        LightEvalContext AreaLight::sample(LightLiSample *lls, float2 u, const SceneData *scene_data) const {
            auto mesh = scene_data->get_mesh(_inst_idx);
            const Distribution1D &distrib = scene_data->distributions[mesh.distribute_idx];
            float PMF = 0;
            size_t triangle_id = distrib.sample_discrete(u.x, &PMF, &u.x);
            float2 bary = square_to_triangle(u);
            LightEvalContext lec;
            lec = scene_data->compute_light_eval_context(_inst_idx, triangle_id, bary);
            return lec;
        }

        LightLiSample AreaLight::Li(LightLiSample lls, const SceneData *data) const {
            float3 wi = lls.lec.pos - lls.lsc.pos;
            lls.wi = normalize(wi);
            lls.L = radiance(LightEvalContext{lls.lec}, -lls.wi, data);
            lls.PDF_dir = PDF_Li(lls.lsc, LightEvalContext{lls.lec}, wi, data);
            return lls;
        }

        float AreaLight::PDF_Li(const LightSampleContext &p_ref, const LightEvalContext &p_light,
                                float3 wi, const SceneData *data) const {
            float3 wo = p_ref.pos - p_light.pos;
            float PDF = luminous::PDF_dir(p_light.PDF_pos, p_light.ng, wo);
            if (is_inf(PDF)) {
                return 0;
            }
            return PDF;
        }

        float AreaLight::inv_area() const {
            return _inv_area;
        }

        Spectrum AreaLight::power() const {
            return (_two_sided ? _2Pi : Pi) * L * (1.f / _inv_area);
        }

        Spectrum AreaLight::radiance(const LightEvalContext &lec, float3 w,
                                     const SceneData *scene_data) const {
            return radiance(lec.uv, lec.ng, w, scene_data);
        }

        Spectrum AreaLight::radiance(float2 uv, float3 ng, float3 w, const SceneData *scene_data) const {
            return (_two_sided || dot(w, ng) > 0) ? L : make_float3(0.f);
        }

        void AreaLight::print() const {
            printf("type:AreaLight,instance id is %u,L:(%f,%f,%f)\n",
                   _inst_idx, L.x, L.y, L.z);
        }


    } //luminous::render
} // luminous::render