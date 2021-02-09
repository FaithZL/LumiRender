//
// Created by Zero on 2021/2/9.
//


#pragma once

#include "../math/common.h"

namespace luminous {
    inline namespace optics {

        template<typename T>
        class RGBSpectrum : public Vector<T, 3>{
        public:
            using scalar_t = T;
            using vector_t = Vector<T, 3>;

            XPU RGBSpectrum(scalar_t r = 0, scalar_t g = 0, scalar_t b = 0)
                    : vector_t(r, g, b) {}

            XPU RGBSpectrum(vector_t vec)
                    : vector_t(vec) {}

            XPU scalar_t R() const noexcept { return x; }

            XPU scalar_t G() const noexcept { return y; }

            XPU scalar_t B() const noexcept { return z; }

            XPU scalar_t luminance() const noexcept {
                return Y();
            };

            XPU scalar_t X() const noexcept {
                return dot(*this, vector_t(0.412453f, 0.357580f, 0.180423f));
            }

            XPU scalar_t Y() const noexcept {
                return dot(*this, vector_t(0.212671f, 0.715160f, 0.072169f));
            }

            XPU scalar_t Z() const noexcept {
                return dot(*this, vector_t(0.019334f, 0.119193f, 0.950227f));
            }

            XPU vector_t XYZ() const noexcept {
                return vector_t(X(), Y(), Z());
            }

            XPU scalar_t average() const noexcept {
                return dot(*this, vector_t(1, 1, 1)) / 3;
            }

            template<typename T>
            XPU static T linear_to_srgb(T L) {
                return select((L < T(0.0031308)),
                              (L * T(12.92)),
                              (T(1.055) * pow(L, T(1.0 / 2.4)) - T(0.055)));
            }

            XPU static RGBSpectrum linear_to_srgb(RGBSpectrum color) {
                vector_t vec = (vector_t)(color);
                return linear_to_srgb(vec);
            }

            template<typename T>
            XPU static T srgb_to_linear(T S) {
                return select((S < T(0.04045)),
                              (S / T(12.92)),
                              (pow((S + 0.055f) * 1.f / 1.055f, (float)2.4f)));
            }

            XPU static RGBSpectrum srgb_to_linear(RGBSpectrum color) {
                vector_t vec = (vector_t)(color);
                return srgb_to_linear(vec);
            }

            XPU bool is_black() const noexcept {
                return is_zero();
            }

        };

        using Spectrum = RGBSpectrum<float>;
    }
}