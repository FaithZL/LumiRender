//
// Created by Zero on 2021/2/4.
//


#pragma once

#include "matrix_types.h"
#include "quaternion.h"
#include "float.h"
#include <cstring>

namespace luminous {
    inline namespace functor {
        using std::abs;
        using std::sqrt;
        using std::pow;
        using std::max;
        using std::min;

        ND_XPU_INLINE bool is_close(float a, float b, float epsilon = 0.00001) {
            return fabsf(a - b) < epsilon;
        }

        template<uint N>
        ND_XPU_INLINE Vector<bool, N> is_close(Vector<float, N> a, Vector<float, N> b,
                                               Vector<float, N> epsilon = Vector<float, N>(0.0001)) {
            return abs(a - b) < epsilon;
        }

#define CHECK_UNIT_VEC(vec) LM_ASSERT(is_close(length(vec), 1.f), "vec is (%f, %f, %f), length is %f\n ", \
                                        (vec).x, (vec).y, (vec).z, length(vec));

        ND_XPU_INLINE float rcp(float f) noexcept { return 1.f / f; }

        ND_XPU_INLINE double rcp(double d) noexcept { return 1. / d; }

        LM_XPU_INLINE float saturate(const float &f) { return std::min(1.f, std::max(0.f, f)); }

        // Vector Functions
#define MAKE_VECTOR_UNARY_FUNC(func)                                          \
    template<typename T, uint N>                                              \
    LM_ND_XPU constexpr auto func(Vector<T, N> v) noexcept {                   \
        static_assert(N == 2 || N == 3 || N == 4);                            \
        if constexpr (N == 2) {                                               \
            return Vector<T, 2>{func(v.x), func(v.y)};                        \
        } else if constexpr (N == 3) {                                        \
            return Vector<T, 3>(func(v.x), func(v.y), func(v.z));             \
        } else {                                                              \
            return Vector<T, 4>(func(v.x), func(v.y), func(v.z), func(v.w));  \
        }                                                                     \
    }

        MAKE_VECTOR_UNARY_FUNC(rcp)

        MAKE_VECTOR_UNARY_FUNC(sign)

        MAKE_VECTOR_UNARY_FUNC(saturate)

        MAKE_VECTOR_UNARY_FUNC(abs)

        MAKE_VECTOR_UNARY_FUNC(sqrt)

        MAKE_VECTOR_UNARY_FUNC(sqr)

#undef MAKE_VECTOR_UNARY_FUNC

        template<typename T, uint32_t N>
        ND_XPU_INLINE bool is_zero(Vector <T, N> v) noexcept {
            return all(v == T(0));
        }

        template<typename T, uint32_t N>
        ND_XPU_INLINE bool nonzero(Vector <T, N> v) noexcept {
            return any(v != T(0));
        }

#undef MAKE_VECTOR_UNARY_FUNC_BOOL

        template<typename T, std::enable_if_t<std::is_unsigned_v<T> && (sizeof(T) == 4u || sizeof(T) == 8u), int> = 0>
        [[nodiscard]] constexpr auto next_pow2(T v) noexcept {
            v--;
            v |= v >> 1u;
            v |= v >> 2u;
            v |= v >> 4u;
            v |= v >> 8u;
            v |= v >> 16u;
            if constexpr (sizeof(T) == 8u) { v |= v >> 32u; }
            return v + 1u;
        }

        template<typename T, uint N, std::enable_if_t<scalar::is_scalar < T>, int> = 0>
        LM_ND_XPU constexpr auto select(Vector<bool, N> pred, Vector <T, N> t, Vector <T, N> f) noexcept {
            static_assert(N == 2 || N == 3 || N == 4);
            if constexpr (N == 2) {
                return Vector < T, N > {select(pred.x, t.x, f.x), select(pred.y, t.y, f.y)};
            } else if constexpr (N == 3) {
                return Vector < T, N > {select(pred.x, t.x, f.x), select(pred.y, t.y, f.y), select(pred.z, t.z, f.z)};
            } else {
                return Vector < T, N > {select(pred.x, t.x, f.x), select(pred.y, t.y, f.y), select(pred.z, t.z, f.z),
                                        select(pred.w, t.w, f.w)};
            }
        }

        template<class T, uint32_t N>
        LM_NODISCARD LM_XPU_INLINE constexpr Vector <T, N>
        vclamp(const Vector <T, N> &v, const Vector <T, N> &low, const Vector <T, N> &upper) {
            static_assert(N >= 2 && N <= 4, "unsupported type!");
            Vector <T, N> ret;
            if constexpr (N == 2) {
                bool2 p0 = {low.x < v.x, low.y < v.y};
                ret = select(p0, v, low);
                p0 = bool2{v.x < upper.x, v.y < upper.y};
                ret = select(p0, v, upper);
            } else if constexpr (N == 3) {
                bool3 p0{low.x < v.x, low.y < v.y, low.z < v.z};
                ret = select(p0, v, low);
                p0 = bool3{v.x < upper.x, v.y < upper.y, v.z < upper.z};
                ret = select(p0, v, upper);
            } else if constexpr (N == 4) {
                bool4 p0 = {low.x < v.x, low.y < v.y, low.z < v.z, low.w < v.w};
                ret = select(p0, v, low);
                p0 = bool4{v.x < upper.x, v.y < upper.y, v.z < upper.z, v.w < upper.w};
                ret = select(p0, v, upper);
            }
            return ret;
        }

#define MAKE_VECTOR_BINARY_FUNC(func)                                                             \
    template<typename T, uint N>                                                                  \
    LM_ND_XPU constexpr auto func(Vector<T, N> v, Vector<T, N> u) noexcept {                       \
        static_assert(N == 2 || N == 3 || N == 4);                                                \
        if constexpr (N == 2) {                                                                   \
            return Vector<T, 2>{func(v.x, u.x), func(v.y, u.y)};                                  \
        } else if constexpr (N == 3) {                                                            \
            return Vector<T, 3>(func(v.x, u.x), func(v.y, u.y), func(v.z, u.z));                  \
        } else {                                                                                  \
            return Vector<T, 4>(func(v.x, u.x), func(v.y, u.y), func(v.z, u.z), func(v.w, u.w));  \
        }                                                                                         \
    }                                                                                             \
    template<typename T, uint N>                                                                  \
    LM_ND_XPU constexpr auto func(T v, Vector<T, N> u) noexcept {                                  \
        static_assert(N == 2 || N == 3 || N == 4);                                                \
        if constexpr (N == 2) {                                                                   \
            return Vector<T, 2>{func(v, u.x), func(v, u.y)};                                      \
        } else if constexpr (N == 3) {                                                            \
            return Vector<T, 3>(func(v, u.x), func(v, u.y), func(v, u.z));                        \
        } else {                                                                                  \
            return Vector<T, 4>(func(v, u.x), func(v, u.y), func(v, u.z), func(v, u.w));          \
        }                                                                                         \
    }                                                                                             \
    template<typename T, uint N>                                                                  \
    LM_ND_XPU constexpr auto func(Vector<T, N> v, T u) noexcept {                                  \
        static_assert(N == 2 || N == 3 || N == 4);                                                \
        if constexpr (N == 2) {                                                                   \
            return Vector<T, 2>{func(v.x, u), func(v.y, u)};                                      \
        } else if constexpr (N == 3) {                                                            \
            return Vector<T, 3>(func(v.x, u), func(v.y, u), func(v.z, u));                        \
        } else {                                                                                  \
            return Vector<T, 4>(func(v.x, u), func(v.y, u), func(v.z, u), func(v.w, u));          \
        }                                                                                         \
    }

        MAKE_VECTOR_BINARY_FUNC(atan2)

        MAKE_VECTOR_BINARY_FUNC(pow)

        MAKE_VECTOR_BINARY_FUNC(min)

        MAKE_VECTOR_BINARY_FUNC(max)

#undef MAKE_VECTOR_UNARY_FUNC

        template<typename T, uint N>
        LM_ND_XPU constexpr auto volume(Vector <T, N> v) noexcept {
            static_assert(N == 2 || N == 3 || N == 4);
            if constexpr (N == 2) {
                return v.x * v.y;
            } else if constexpr (N == 3) {
                return v.x * v.y * v.z;
            } else {
                return v.x * v.y * v.z * v.w;
            }
        }

        template<typename T, uint N>
        LM_ND_XPU constexpr auto dot(Vector <T, N> u, Vector <T, N> v) noexcept {
            static_assert(N == 2 || N == 3 || N == 4);
            if constexpr (N == 2) {
                return u.x * v.x + u.y * v.y;
            } else if constexpr (N == 3) {
                return u.x * v.x + u.y * v.y + u.z * v.z;
            } else {
                return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
            }
        }

        template<typename T, uint N>
        LM_ND_XPU constexpr auto abs_dot(Vector <T, N> u, Vector <T, N> v) noexcept {
            return abs(dot(u, v));
        }

        template<typename T, uint N>
        LM_ND_XPU constexpr auto length(Vector <T, N> u) noexcept {
            return sqrt(dot(u, u));
        }

        template<typename T, uint N>
        LM_ND_XPU constexpr auto length_squared(Vector <T, N> u) noexcept {
            return dot(u, u);
        }

        template<typename T, uint N>
        LM_ND_XPU constexpr auto normalize(Vector <T, N> u) noexcept {
#ifndef __CUDACC__
            return u * (1.0f / length(u));
#else
            return u * ::rsqrt(dot(u,u));
#endif
        }

        template<typename T, uint N>
        LM_ND_XPU constexpr auto distance(Vector <T, N> u, Vector <T, N> v) noexcept {
            return length(u - v);
        }

        template<typename T, uint N>
        LM_ND_XPU constexpr auto distance_squared(Vector <T, N> u, Vector <T, N> v) noexcept {
            return length_squared(u - v);
        }

        template<typename T>
        LM_ND_XPU constexpr auto cross(Vector<T, 3> u, Vector<T, 3> v) noexcept {
            return Vector<T, 3>(u.y * v.z - v.y * u.z,
                                u.z * v.x - v.z * u.x,
                                u.x * v.y - v.x * u.y);
        }

        template<typename T, uint N>
        ND_XPU_INLINE T triangle_area(Vector <T, N> p0, Vector <T, N> p1, Vector <T, N> p2) noexcept {
            static_assert(N == 3 || N == 2, "N must be greater than 1!");
            if constexpr (N == 2) {
                Vector<T, 3> pp0 = Vector<T, 3>{p0.x, p0.y, 0};
                Vector<T, 3> pp1 = Vector<T, 3>{p1.x, p1.y, 0};
                Vector<T, 3> pp2 = Vector<T, 3>{p2.x, p2.y, 0};
                return 0.5 * length(cross(pp1 - pp0, pp2 - pp0));
            } else {
                return 0.5 * length(cross(p1 - p0, p2 - p0));
            }
        }

        template<typename T, typename F2>
        LM_ND_XPU T triangle_lerp(F2 barycentric, T v0, T v1, T v2) noexcept {
            auto u = barycentric.x;
            auto v = barycentric.y;
            auto w = 1 - barycentric.x - barycentric.y;
            return u * v0 + v * v1 + w * v2;
        }

        template<typename T>
        LM_XPU Vector<T, 3> face_forward(Vector<T, 3> v1, Vector<T, 3> v2) noexcept {
            return dot(v1, v2) > 0 ? v1 : -v1;
        }

        template<typename T>
        LM_XPU void coordinate_system(Vector<T, 3> v1,
                                      Vector<T, 3> *v2,
                                      Vector<T, 3> *v3) noexcept {
            CHECK_UNIT_VEC(v1)
            if (abs(v1.x) > abs(v1.y)) {
                *v2 = Vector<T, 3>(-v1.z, 0, v1.x) / sqrt(v1.x * v1.x + v1.z * v1.z);
            } else {
                *v2 = Vector<T, 3>(0, v1.z, -v1.y) / sqrt(v1.y * v1.y + v1.z * v1.z);
            }
            *v3 = cross(v1, *v2);
        }

        // Quaternion Functions
        LM_NODISCARD LM_XPU_INLINE float dot(Quaternion q1, Quaternion q2) noexcept {
            return dot(q1.v, q2.v) + q1.w * q2.w;
        }

        LM_NODISCARD LM_XPU_INLINE Quaternion normalize(Quaternion q) noexcept {
            return q / std::sqrt(dot(q, q));
        }

        ND_XPU_INLINE float sin_x_over_x(float x) noexcept {
            if (1 + x * x == 1) {
                return 1;
            }
            return std::sin(x) / x;
        }

        ND_XPU_INLINE float sinc(float x) noexcept {
            return sin_x_over_x(Pi * x);
        }

        ND_XPU_INLINE float lanczos(float x, float tau) {
            x = std::abs(x);
            if (x < 1e-5f) {
                return 1;
            }
            if (x > 1.f) {
                return 0;
            }
            x *= Pi;
            float s = sin_x_over_x(x * tau);
            float lanczos = sin_x_over_x(x);
            return s * lanczos;
        }

        ND_XPU_INLINE float windowed_sinc(float x, float radius, float tau) {
            if (std::abs(x) > radius) {
                return 0;
            }
            return sinc(x) * sinc(x / tau);
        }

        LM_NODISCARD LM_XPU_INLINE Quaternion slerp(float t, const Quaternion &q1, const Quaternion &q2) noexcept {
            float cosTheta = dot(q1, q2);
            if (cosTheta > .9995f)
                //If the rotation Angle is too small, treat it as a straight line
                return normalize(q1 * (1 - t) + q2 * t);
            else {
                // origin equation: result = (q1 * sin((1-t)*θ) + q2 * sin(tθ)) / (sinθ)
                // More intuitive understanding : 
                // qperp = q2 - cosθ * q1 = q2 - dot(q1, q2) * q1
                // q' = q1 * cos(θt) + qperp * sin(θt)
                float theta = std::acos(clamp(cosTheta, -1, 1));
                float theta_p = theta * t;
                Quaternion q_perp = normalize(q2 - q1 * cosTheta);
                return q1 * std::cos(theta_p) + q_perp * std::sin(theta_p);
            }
        }

        // Matrix Functions
        template<typename T>
        LM_NODISCARD LM_XPU constexpr auto transpose(Matrix3x3 <T> m) noexcept {
            return Matrix3x3<T>(m[0].x, m[1].x, m[2].x,
                                m[0].y, m[1].y, m[2].y,
                                m[0].z, m[1].z, m[2].z);
        }

        template<typename T>
        LM_NODISCARD LM_XPU constexpr auto transpose(Matrix4x4 <T> m) noexcept {
            return Matrix4x4<T>(m[0].x, m[1].x, m[2].x, m[3].x,
                                m[0].y, m[1].y, m[2].y, m[3].y,
                                m[0].z, m[1].z, m[2].z, m[3].z,
                                m[0].w, m[1].w, m[2].w, m[3].w);
        }

        template<typename T>
        LM_NODISCARD LM_XPU auto inverse(Matrix3x3 <T> m) noexcept {// from GLM
            T one_over_determinant = 1.0f / (m[0].x * (m[1].y * m[2].z - m[2].y * m[1].z) -
                                             m[1].x * (m[0].y * m[2].z - m[2].y * m[0].z) +
                                             m[2].x * (m[0].y * m[1].z - m[1].y * m[0].z));
            return Matrix3x3<T>(
                    (m[1].y * m[2].z - m[2].y * m[1].z) * one_over_determinant,
                    (m[2].y * m[0].z - m[0].y * m[2].z) * one_over_determinant,
                    (m[0].y * m[1].z - m[1].y * m[0].z) * one_over_determinant,
                    (m[2].x * m[1].z - m[1].x * m[2].z) * one_over_determinant,
                    (m[0].x * m[2].z - m[2].x * m[0].z) * one_over_determinant,
                    (m[1].x * m[0].z - m[0].x * m[1].z) * one_over_determinant,
                    (m[1].x * m[2].y - m[2].x * m[1].y) * one_over_determinant,
                    (m[2].x * m[0].y - m[0].x * m[2].y) * one_over_determinant,
                    (m[0].x * m[1].y - m[1].x * m[0].y) * one_over_determinant);
        }

        template<typename T>
        LM_NODISCARD LM_XPU constexpr auto inverse(const Matrix4x4 <T> m) noexcept {// from GLM
            const T coef00 = m[2].z * m[3].w - m[3].z * m[2].w;
            const T coef02 = m[1].z * m[3].w - m[3].z * m[1].w;
            const T coef03 = m[1].z * m[2].w - m[2].z * m[1].w;
            const T coef04 = m[2].y * m[3].w - m[3].y * m[2].w;
            const T coef06 = m[1].y * m[3].w - m[3].y * m[1].w;
            const T coef07 = m[1].y * m[2].w - m[2].y * m[1].w;
            const T coef08 = m[2].y * m[3].z - m[3].y * m[2].z;
            const T coef10 = m[1].y * m[3].z - m[3].y * m[1].z;
            const T coef11 = m[1].y * m[2].z - m[2].y * m[1].z;
            const T coef12 = m[2].x * m[3].w - m[3].x * m[2].w;
            const T coef14 = m[1].x * m[3].w - m[3].x * m[1].w;
            const T coef15 = m[1].x * m[2].w - m[2].x * m[1].w;
            const T coef16 = m[2].x * m[3].z - m[3].x * m[2].z;
            const T coef18 = m[1].x * m[3].z - m[3].x * m[1].z;
            const T coef19 = m[1].x * m[2].z - m[2].x * m[1].z;
            const T coef20 = m[2].x * m[3].y - m[3].x * m[2].y;
            const T coef22 = m[1].x * m[3].y - m[3].x * m[1].y;
            const T coef23 = m[1].x * m[2].y - m[2].x * m[1].y;
            const Vector<T, 4> fac0 = Vector<T, 4>(coef00, coef00, coef02, coef03);
            const Vector<T, 4> fac1 = Vector<T, 4>(coef04, coef04, coef06, coef07);
            const Vector<T, 4> fac2 = Vector<T, 4>(coef08, coef08, coef10, coef11);
            const Vector<T, 4> fac3 = Vector<T, 4>(coef12, coef12, coef14, coef15);
            const Vector<T, 4> fac4 = Vector<T, 4>(coef16, coef16, coef18, coef19);
            const Vector<T, 4> fac5 = Vector<T, 4>(coef20, coef20, coef22, coef23);
            const Vector<T, 4> Vec0 = Vector<T, 4>(m[1].x, m[0].x, m[0].x, m[0].x);
            const Vector<T, 4> Vec1 = Vector<T, 4>(m[1].y, m[0].y, m[0].y, m[0].y);
            const Vector<T, 4> Vec2 = Vector<T, 4>(m[1].z, m[0].z, m[0].z, m[0].z);
            const Vector<T, 4> Vec3 = Vector<T, 4>(m[1].w, m[0].w, m[0].w, m[0].w);
            const Vector<T, 4> inv0 = Vec1 * fac0 - Vec2 * fac1 + Vec3 * fac2;
            const Vector<T, 4> inv1 = Vec0 * fac0 - Vec2 * fac3 + Vec3 * fac4;
            const Vector<T, 4> inv2 = Vec0 * fac1 - Vec1 * fac3 + Vec3 * fac5;
            const Vector<T, 4> inv3 = Vec0 * fac2 - Vec1 * fac4 + Vec2 * fac5;
            const Vector<T, 4> sign_a = Vector<T, 4>((T) +1, (T) -1, (T) +1, (T) -1);
            const Vector<T, 4> sign_b = Vector<T, 4>((T) -1, (T) +1, (T) -1, (T) +1);
            const Vector<T, 4> inv_0 = inv0 * sign_a;
            const Vector<T, 4> inv_1 = inv1 * sign_b;
            const Vector<T, 4> inv_2 = inv2 * sign_a;
            const Vector<T, 4> inv_3 = inv3 * sign_b;
            const Vector<T, 4> dot0 = m[0] * Vector<T, 4>(inv_0.x, inv_1.x, inv_2.x, inv_3.x);
            const T dot1 = dot0.x + dot0.y + dot0.z + dot0.w;
            const T one_over_determinant = 1 / dot1;
            return Matrix4x4<T>(inv_0 * one_over_determinant,
                                inv_1 * one_over_determinant,
                                inv_2 * one_over_determinant,
                                inv_3 * one_over_determinant);
        }

        // other functions
        template<class To, class From>
        LM_XPU typename std::enable_if_t<sizeof(To) == sizeof(From) &&
                                         std::is_trivially_copyable_v<From> &&
                                         std::is_trivially_copyable_v<To>, To>
        bit_cast(const From &src) noexcept {
            static_assert(std::is_trivially_constructible_v<To>,
                          "This implementation requires the destination type to be trivially "
                          "constructible");
            To dst;
            std::memcpy(&dst, &src, sizeof(To));
            return dst;
        }

        LM_ND_XPU inline float fast_exp(float x) {
#ifdef IS_GPU_CODE
            return __expf(x);
#else
            return std::exp(x);
#endif
        }

        LM_NODISCARD LM_XPU_INLINE float gaussian(float x, float mu = 0, float sigma = 1) {
            return 1 / std::sqrt(2 * Pi * sigma * sigma) *
                   fast_exp(-sqr(x - mu) / (2 * sigma * sigma));
        }

        LM_NODISCARD LM_XPU_INLINE float gaussian_integral(float x0, float x1, float mu = 0,
                                                           float sigma = 1) {
            assert(sigma > 0);
            float sigmaRoot2 = sigma * float(1.414213562373095);
            return 0.5f * (std::erf((mu - x0) / sigmaRoot2) - std::erf((mu - x1) / sigmaRoot2));
        }


        template<typename Predicate>
        LM_NODISCARD LM_XPU inline size_t find_interval(size_t sz, const Predicate &pred) {
            using ssize_t = std::make_signed_t<size_t>;
            ssize_t size = (ssize_t) sz - 2, first = 1;
            while (size > 0) {
                // Evaluate predicate at midpoint and update _first_ and _size_
                size_t half = (size_t) size >> 1, middle = first + half;
                bool pred_result = pred(middle);
                first = pred_result ? middle + 1 : first;
                size = pred_result ? size - (half + 1) : half;
            }
            return (size_t) clamp((ssize_t) first - 1, 0, sz - 2);
        }

    } // luminous::functors
} // luminous