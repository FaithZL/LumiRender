//
// Created by Zero on 2021/7/28.
//

#include "envmap.h"
#include "render/scene/scene_data.h"
#include "core/refl/factory.h"

namespace luminous {
    inline namespace render {

        LightLiSample Envmap::Li(LightLiSample lls, const SceneData *data) const {
            lls.set_Li(L(_w2o.apply_vector(lls.wi), data));
            return lls;
        }

        LightEvalContext Envmap::sample(LightLiSample *lls, float2 u, const SceneData *scene_data) const {
            const Distribution2D &distribution2d = scene_data->get_distribution2d(_distribution_idx);
            float map_PDF = 0;
            int2 offset;
            float2 uv = distribution2d.sample_continuous(u, &map_PDF, &offset);
            if (map_PDF == 0) {
                lls->PDF_dir = 0;
                return {};
            }
            float theta = uv[1] * Pi;
            float phi = uv[0] * _2Pi;
            float sin_theta = 0, cos_theta = 0;
            sincos(theta, &sin_theta, &cos_theta);
            float3 dir_in_world = _w2o.inverse().apply_vector(spherical_direction(sin_theta, cos_theta, phi));
            float PDF_dir = map_PDF / (2 * Pi * Pi * sin_theta);
            float3 pos = lls->lsc.pos + dir_in_world;
            LightEvalContext lec{pos, make_float3(0.f), make_float2(0.f), 0.f};
            float3 wi = normalize(dir_in_world);
            lls->set_sample_result(PDF_dir, lec, wi);
            return lec;
        }

        Spectrum Envmap::L(float3 dir_in_obj, const SceneData *data) const {
            float2 uv = make_float2(spherical_phi(dir_in_obj) * inv2Pi, spherical_theta(dir_in_obj) * invPi);
            float4 ret = _emission.eval(data, uv);
            return {make_float3(ret)};
        }

        Spectrum Envmap::on_miss(float3 dir, const SceneData *data) const {
            float3 d = _w2o.apply_vector(normalize(dir));
            return L(d, data);
        }

        float Envmap::PDF_Li(const LightSampleContext &p_ref, const LightEvalContext &p_light,
                             float3 wi, const SceneData *data) const {
            float3 w = _w2o.apply_vector(wi);
            float theta = spherical_theta(w);
            float phi = spherical_phi(w);
            float sin_theta = std::sin(theta);
            if (sin_theta == 0) {
                return 0;
            }
            const Distribution2D &distribution2d = data->get_distribution2d(_distribution_idx);
            float2 uv = make_float2(phi * inv2Pi, theta * invPi);
            return distribution2d.PDF(uv) / (_2Pi * Pi * sin_theta);
        }

        Spectrum Envmap::power() const {
            // todo
            return luminous::Spectrum(1.f);
        }

        void Envmap::print() const {
            printf("type:Envmap\n");
        }

        CPU_ONLY(std::vector<float> Envmap::create_distribution(const Image &image) {
            auto width = image.width();
            auto height = image.height();
            std::vector<float> ret(image.pixel_num(), 0);
            image.for_each_pixel([&](const std::byte *pixel, index_t idx) {
                float f = 0;
                float v = idx / width + 0.5f;
                float theta = v / height;
                float sinTheta = std::sin(Pi * theta);
                switch (image.pixel_format()) {
                    case PixelFormat::RGBA32F: {
                        float4 val = *(reinterpret_cast<const float4 *>(pixel));
                        f = luminance(val);
                        break;
                    }
                    case PixelFormat::RGBA8U: {
                        uchar4 val = *(reinterpret_cast<const uchar4 *>(pixel));
                        float4 f4 = make_float4(val) / 255.f;
                        f = luminance(f4);
                        break;
                    }
                    default:
                        break;
                }
                ret[idx] = f * sinTheta;
            });
            return ret;
        })

    }
}