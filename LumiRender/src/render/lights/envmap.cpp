//
// Created by Zero on 2021/7/28.
//

#include "envmap.h"


namespace luminous {
    inline namespace render {

        LightLiSample Envmap::Li(LightLiSample lls, const SceneData *data) const {
            return LightLiSample();
        }

        SurfaceInteraction Envmap::sample(LightLiSample lls, float2 u, const SceneData *scene_data) const {
            const Distribution2D &distribution2d = scene_data->get_distribution2d(_distribution_idx);
            float PDF;
            float2 uv = distribution2d.sample_continuous(u, &PDF);
            float theta = uv[1] * Pi;
            float phi = uv[0] * _2Pi;
            float sin_theta, cos_theta;
            sincos(theta, &sin_theta, &cos_theta);
            float3 dir_in_world = _w2o.inverse().apply_vector(spherical_direction(sin_theta, cos_theta, phi));

            return SurfaceInteraction();
        }

        Spectrum Envmap::L(float3 dir_in_obj, const SceneData *data) const {
            const Texture &tex = data->textures[_tex_idx];
            float2 uv = make_float2(spherical_phi(dir_in_obj) * inv2Pi, spherical_theta(dir_in_obj) * invPi);
            float4 ret = tex.eval(uv);
            return {make_float3(ret)};
        }

        Spectrum Envmap::on_miss(Ray ray, const SceneData *data) const {
            float3 d = normalize(_w2o.apply_vector(ray.direction()));
            return L(d, data);
        }

        float Envmap::PDF_Li(const Interaction &p_ref, const SurfaceInteraction &p_light) const {
            return 0;
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
            auto func = [&](const std::byte *pixel, index_t idx) {
                float f = 0;
                float v = idx / width + 0.5f;
                float theta = v / height;
                float sinTheta = std::sin(Pi * theta);
                switch (image.pixel_format()) {
                    case PixelFormat::RGBA32F:{
                        float4 val = *(reinterpret_cast<const float4*>(pixel));
                        f = luminance(val);
                        break;
                    }
                    case PixelFormat::RGBA8U:{
                        uchar4 val = *(reinterpret_cast<const uchar4*>(pixel));
                        float4 f4 = make_float4(val) / 255.f;
                        f = luminance(f4);
                        break;
                    }
                    default:
                        break;
                }
                ret[idx] = f * sinTheta;
            };
            image.for_each_pixel(func);
            return ret;
        })

        CPU_ONLY(Envmap Envmap::create(const LightConfig &config) {
            Transform o2w = config.o2w_config.create();
            Transform rotate_x = Transform::rotation_x(90);
            o2w = o2w * rotate_x;
            return Envmap(config.texture_config.tex_idx, o2w.inverse(),
                          config.distribution_idx, config.scene_box);
        })

    }
}