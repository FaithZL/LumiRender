//
// Created by Zero on 2021/2/4.
//


#pragma once

#include <type_traits>
#include "scalar_types.h"
#include "../string_util.h"

namespace luminous {

    inline namespace math {

        namespace detail {

            template<typename T, uint32_t N>
            constexpr auto vector_alignment = sizeof(T) * (N == 3 ? 4 : N);

            template<typename T, uint32_t>
            struct VectorStorage {
            };

            template<typename T>
            struct VectorStorage<T, 2> {
                T x, y;
                using scalar_t = T;
                XPU constexpr VectorStorage() noexcept: x{}, y{} {}

                XPU explicit constexpr VectorStorage(T s) noexcept: x{s}, y{s} {}

                XPU explicit constexpr VectorStorage(T x, T y) noexcept: x{x}, y{y} {}
            };

            template<typename T>
            struct VectorStorage<T, 3> {
                T x, y, z;
                using scalar_t = T;
                XPU constexpr VectorStorage() noexcept: x{}, y{}, z{} {}

                XPU explicit constexpr VectorStorage(T s) noexcept: x{s}, y{s}, z{s} {}

                XPU explicit constexpr VectorStorage(T x, T y, T z) noexcept: x{x}, y{y}, z{z} {}
            };

            template<typename T>
            struct VectorStorage<T, 4> {
                T x, y, z, w;
                using scalar_t = T;
                XPU constexpr VectorStorage() noexcept: x{}, y{}, z{}, w{} {}

                XPU explicit constexpr VectorStorage(T s) noexcept: x{s}, y{s}, z{s}, w{s} {}

                XPU explicit constexpr VectorStorage(T x, T y, T z, T w) noexcept: x{x}, y{y}, z{z}, w{w} {}
            };

        }// namespace detail

        template<typename T, uint32_t N>
        struct alignas(detail::vector_alignment<T, N>) Vector : detail::VectorStorage<T, N> {

            using Storage = detail::VectorStorage<T, N>;

            XPU constexpr Vector() noexcept: detail::VectorStorage<T, N>{static_cast<T>(0)} {}

            XPU explicit constexpr Vector(T u) noexcept: detail::VectorStorage<T, N>{u} {}

            template<typename... U>
            XPU explicit constexpr Vector(U... u) noexcept : detail::VectorStorage<T, N>{u...} {}

            template<typename Index>
            NDSC_XPU T &operator[](Index i) noexcept { return reinterpret_cast<T(&)[N]>(*this)[i]; }

            template<typename Index>
            NDSC_XPU T operator[](Index i) const noexcept { return reinterpret_cast<const T(&)[N]>(*this)[i]; }

            XPU void print() const noexcept {
                static_assert(N == 2 || N == 3 || N == 4);
                if constexpr (N == 2) {
                    if constexpr (std::is_same_v<T, float>) {
                        printf("%f,%f \n", Storage::x, Storage::y);
                    } else if constexpr(std::is_same_v<T, int>) {
                        printf("%d,%d \n", Storage::x, Storage::y);
                    } else if constexpr(std::is_same_v<T, uint>) {
                        printf("%u,%u \n", Storage::x, Storage::y);
                    } else {
                        assert(0);
                    }
                } else if constexpr (N == 3) {
                    if constexpr (std::is_same_v<T, float>) {
                        printf("%f,%f,%f \n", Storage::x, Storage::y, Storage::z);
                    } else if constexpr(std::is_same_v<T, int>) {
                        printf("%d,%d,%d \n", Storage::x, Storage::y, Storage::z);
                    } else if constexpr(std::is_same_v<T, uint>) {
                        printf("%u,%u,%u \n", Storage::x, Storage::y, Storage::z);
                    } else {
                        assert(0);
                    }
                } else {
                    if constexpr (std::is_same_v<T, float>) {
                        printf("%f,%f,%f,%f \n", Storage::x, Storage::y, Storage::z, Storage::w);
                    } else if constexpr(std::is_same_v<T, int>) {
                        printf("%d,%d,%d,%d \n", Storage::x, Storage::y, Storage::z, Storage::w);
                    } else if constexpr(std::is_same_v<T, uint>) {
                        printf("%u,%u,%u,%u \n", Storage::x, Storage::y, Storage::z, Storage::w);
                    } else {
                        assert(0);
                    }
                }
            }

            GEN_STRING_FUNC({
                static_assert(N == 2 || N == 3 || N == 4);
                if constexpr (N == 2) {
                    return serialize("[", Storage::x, ",", Storage::y, "]");
                } else if constexpr (N == 3) {
                    return serialize("[", Storage::x, ",", Storage::y, ",", Storage::z, "]");
                } else {
                    return serialize("[", Storage::x, ",", Storage::y, ",", Storage::z, ",", Storage::w, "]");
                }
            })

#define MAKE_ASSIGN_OP(op)                           \
    XPU Vector &operator op(Vector<T, N> rhs) noexcept { \
        static_assert(N == 2 || N == 3 || N == 4);   \
        if constexpr (N == 2) {                      \
            Storage::x op rhs.x;                     \
            Storage::y op rhs.y;                     \
        } else if constexpr (N == 3) {               \
            Storage::x op rhs.x;                     \
            Storage::y op rhs.y;                     \
            Storage::z op rhs.z;                     \
        } else {                                     \
            Storage::x op rhs.x;                     \
            Storage::y op rhs.y;                     \
            Storage::z op rhs.z;                     \
            Storage::w op rhs.w;                     \
        }                                            \
        return *this;                                \
    }                                                \
    XPU Vector &operator op(T rhs) noexcept {        \
        static_assert(N == 2 || N == 3 || N == 4);   \
        if constexpr (N == 2) {                      \
            Storage::x op rhs;                       \
            Storage::y op rhs;                       \
        } else if constexpr (N == 3) {               \
            Storage::x op rhs;                       \
            Storage::y op rhs;                       \
            Storage::z op rhs;                       \
        } else {                                     \
            Storage::x op rhs;                       \
            Storage::y op rhs;                       \
            Storage::z op rhs;                       \
            Storage::w op rhs;                       \
        }                                            \
        return *this;                                \
    }

            MAKE_ASSIGN_OP(+=)

            MAKE_ASSIGN_OP(-=)

            MAKE_ASSIGN_OP(*=)

            MAKE_ASSIGN_OP(/=)

            MAKE_ASSIGN_OP(%=)

#undef MAKE_ASSIGN_OP
        };


#define MAKE_VECTOR_UNARY_OP(op)                                                         \
    template<typename T, uint N>                                                         \
    XPU [[nodiscard]] constexpr Vector<std::decay_t<decltype(op static_cast<T>(0))>, N>  \
    operator op(Vector<T, N> v) noexcept {                                               \
        static_assert(N == 2 || N == 3 || N == 4);                                       \
        if constexpr (N == 2) {                                                          \
            return Vector<T, N>{op v.x, op v.y};                                         \
        } else if constexpr (N == 3) {                                                   \
            return Vector<T, N>{op v.x, op v.y, op v.z};                                 \
        } else {                                                                         \
            return Vector<T, N>{op v.x, op v.y, op v.z, op v.w};                         \
        }                                                                                \
    }


        MAKE_VECTOR_UNARY_OP(-)

        MAKE_VECTOR_UNARY_OP(+)

        MAKE_VECTOR_UNARY_OP(!)

        MAKE_VECTOR_UNARY_OP(~)

#undef MAKE_VECTOR_UNARY_OP

#define MAKE_VECTOR_BINARY_OP(op)                                                                 \
    template<typename T, uint32_t N, std::enable_if_t<scalar::is_scalar<T>, int> = 0>             \
    XPU constexpr Vector<T, N> operator op(Vector<T, N> lhs, Vector<T, N> rhs) noexcept {         \
        static_assert(N == 2 || N == 3 || N == 4);                                                \
        if constexpr (N == 2) {                                                                   \
            return Vector<T, 2>{lhs.x op rhs.x, lhs.y op rhs.y};                                  \
        } else if constexpr (N == 3) {                                                            \
            return Vector<T, 3>{lhs.x op rhs.x, lhs.y op rhs.y, lhs.z op rhs.z};                  \
        } else {                                                                                  \
            return Vector<T, 4>{lhs.x op rhs.x, lhs.y op rhs.y, lhs.z op rhs.z, lhs.w op rhs.w};  \
        }                                                                                         \
    }                                                                                             \
    template<typename T, uint32_t N, std::enable_if_t<scalar::is_scalar<T>, int> = 0>             \
    XPU constexpr Vector<T, N> operator op(T lhs, Vector<T, N> rhs) noexcept {                    \
        static_assert(N == 2 || N == 3 || N == 4);                                                \
        if constexpr (N == 2) {                                                                   \
            return Vector<T, 2>{lhs op rhs.x, lhs op rhs.y};                                      \
        } else if constexpr (N == 3) {                                                            \
            return Vector<T, 3>{lhs op rhs.x, lhs op rhs.y, lhs op rhs.z};                        \
        } else {                                                                                  \
            return Vector<T, 4>{lhs op rhs.x, lhs op rhs.y, lhs op rhs.z, lhs op rhs.w};          \
        }                                                                                         \
    }                                                                                             \
    template<typename T, uint32_t N, std::enable_if_t<scalar::is_scalar<T>, int> = 0>             \
    XPU constexpr Vector<T, N> operator op(Vector<T, N> lhs, T rhs) noexcept {                    \
        static_assert(N == 2 || N == 3 || N == 4);                                                \
        if constexpr (N == 2) {                                                                   \
            return Vector<T, 2>{lhs.x op rhs, lhs.y op rhs};                                      \
        } else if constexpr (N == 3) {                                                            \
            return Vector<T, 3>{lhs.x op rhs, lhs.y op rhs, lhs.z op rhs};                        \
        } else {                                                                                  \
            return Vector<T, 4>{lhs.x op rhs, lhs.y op rhs, lhs.z op rhs, lhs.w op rhs};          \
        }                                                                                         \
    }

        MAKE_VECTOR_BINARY_OP(+)

        MAKE_VECTOR_BINARY_OP(-)

        MAKE_VECTOR_BINARY_OP(*)

        MAKE_VECTOR_BINARY_OP(/)

        MAKE_VECTOR_BINARY_OP(%)

#undef MAKE_VECTOR_BINARY_OP

#define MAKE_VECTOR_RELATIONAL_OP(op)                                                                \
    template<typename T, uint N>                                                                     \
    XPU constexpr auto operator op(Vector<T, N> lhs, Vector<T, N> rhs) noexcept {                    \
        static_assert(N == 2 || N == 3 || N == 4);                                                   \
        if constexpr (N == 2) {                                                                      \
            return Vector<bool, 2>{lhs.x op rhs.x, lhs.y op rhs.y};                                  \
        } else if constexpr (N == 3) {                                                               \
            return Vector<bool, 3>{lhs.x op rhs.x, lhs.y op rhs.y, lhs.z op rhs.z};                  \
        } else {                                                                                     \
            return Vector<bool, 4>{lhs.x op rhs.x, lhs.y op rhs.y, lhs.z op rhs.z, lhs.w op rhs.w};  \
        }                                                                                            \
    }                                                                                                \
    template<typename T, uint N>                                                                     \
    XPU constexpr auto operator op(T lhs, Vector<T, N> rhs) noexcept {                               \
        static_assert(N == 2 || N == 3 || N == 4);                                                   \
        if constexpr (N == 2) {                                                                      \
            return Vector<bool, 2>{lhs op rhs.x, lhs op rhs.y};                                      \
        } else if constexpr (N == 3) {                                                               \
            return Vector<bool, 3>{lhs op rhs.x, lhs op rhs.y, lhs op rhs.z};                        \
        } else {                                                                                     \
            return Vector<bool, 4>{lhs op rhs.x, lhs op rhs.y, lhs op rhs.z, lhs op rhs.w};          \
        }                                                                                            \
    }                                                                                                \
    template<typename T, uint N>                                                                     \
    XPU constexpr auto operator op(Vector<T, N> lhs, T rhs) noexcept {                               \
        static_assert(N == 2 || N == 3 || N == 4);                                                   \
        if constexpr (N == 2) {                                                                      \
            return Vector<bool, 2>{lhs.x op rhs, lhs.y op rhs};                                      \
        } else if constexpr (N == 3) {                                                               \
            return Vector<bool, 3>{lhs.x op rhs, lhs.y op rhs, lhs.z op rhs};                        \
        } else {                                                                                     \
            return Vector<bool, 4>{lhs.x op rhs, lhs.y op rhs, lhs.z op rhs, lhs.w op rhs};          \
        }                                                                                            \
    }

        MAKE_VECTOR_RELATIONAL_OP(==)

        MAKE_VECTOR_RELATIONAL_OP(!=)

        MAKE_VECTOR_RELATIONAL_OP(<)

        MAKE_VECTOR_RELATIONAL_OP(<=)

        MAKE_VECTOR_RELATIONAL_OP(>)

        MAKE_VECTOR_RELATIONAL_OP(>=)

#undef MAKE_VECTOR_RELATIONAL_OP

#define MAKE_VECTOR_MAKE_TYPE2(type)                                                      \
    XPU constexpr auto make_##type##2() noexcept { return type##2{}; }                    \
    XPU constexpr auto make_##type##2(type s) noexcept { return type##2{s}; }             \
    XPU constexpr auto make_##type##2(type x, type y) noexcept { return type##2{x, y}; }  \
    template<typename T>                                                                  \
    XPU constexpr auto make_##type##2(T v[]) noexcept {                                   \
        return type##2{static_cast<type>(v[0]), static_cast<type>(v[1])};                 \
    }                                                                                     \
                                                                                          \
    template<typename U, uint N>                                                          \
    XPU constexpr auto make_##type##2(Vector<U, N> v) noexcept {                          \
        static_assert(N == 2 || N == 3 || N == 4);                                        \
        return type##2{static_cast<type>(v.x), static_cast<type>(v.y)};                   \
    }

#define MAKE_VECTOR_MAKE_TYPE3(type)                                                                   \
    XPU constexpr auto make_##type##3() noexcept { return type##3{}; }                                 \
    XPU constexpr auto make_##type##3(type s) noexcept { return type##3{s}; }                          \
    XPU constexpr auto make_##type##3(type x, type y, type z) noexcept { return type##3 {x, y, z}; }   \
    XPU constexpr auto make_##type##3(type##2 v, type z) noexcept { return type##3 {v.x, v.y, z}; }    \
    XPU constexpr auto make_##type##3(type x, type##2 v) noexcept { return type##3 {x, v.x, v.y}; }    \
                                                                                                       \
    template<typename U, uint N>                                                                       \
    XPU constexpr auto make_##type##3(Vector<U, N> v) noexcept {                                       \
        static_assert(N == 3 || N == 4);                                                               \
        return type##3 {static_cast<type>(v.x), static_cast<type>(v.y), static_cast<type>(v.z)};       \
    }                                                                                                  \
                                                                                                       \
    template<typename U>                                                                               \
    XPU constexpr auto make_##type##3(U v[]) noexcept {                                                \
        return type##3{static_cast<type>(v[0]), static_cast<type>(v[1]), static_cast<type>(v[2])};     \
    }                                                                                                  \

#define MAKE_VECTOR_MAKE_TYPE4(type)                                                                             \
    XPU constexpr auto make_##type##4() noexcept { return type##4 {}; }                                          \
    XPU constexpr auto make_##type##4(type s) noexcept { return type##4 {s}; }                                   \
    XPU constexpr auto make_##type##4(type x, type y, type z, type w) noexcept { return type##4 {x, y, z, w}; }  \
    XPU constexpr auto make_##type##4(type##2 v, type z, type w) noexcept { return type##4 {v.x, v.y, z, w}; }   \
    XPU constexpr auto make_##type##4(type x, type y, type##2 v) noexcept { return type##4 {x, y, v.x, v.y}; }   \
    XPU constexpr auto make_##type##4(type x, type##2 v, type w) noexcept { return type##4 {x, v.x, v.y, w}; }   \
    XPU constexpr auto make_##type##4(type##2 v, type##2 u) noexcept { return type##4 {v.x, v.y, u.x, u.y}; }    \
    XPU constexpr auto make_##type##4(type##3 v, type w) noexcept { return type##4 {v.x, v.y, v.z, w}; }         \
    XPU constexpr auto make_##type##4(type x, type##3 v) noexcept { return type##4 {x, v.x, v.y, v.z}; }         \
    template<typename U>                                                                                         \
    XPU constexpr auto make_##type##4(Vector<U, 4> v) noexcept {                                                 \
        return type##4 {                                                                                         \
            static_cast<type>(v.x),                                                                              \
            static_cast<type>(v.y),                                                                              \
            static_cast<type>(v.z),                                                                              \
            static_cast<type>(v.w)};                                                                             \
    }                                                                                                            \
                                                                                                                 \
    template<typename U>                                                                                         \
    XPU constexpr auto make_##type##4(U v[]) noexcept {                                                          \
        return type##4 {                                                                                         \
            static_cast<type>(v[0]),                                                                             \
            static_cast<type>(v[1]),                                                                             \
            static_cast<type>(v[2]),                                                                             \
            static_cast<type>(v[3])};                                                                            \
    }


#define MAKE_VECTOR_TYPE(type)        \
    using type##2 = Vector<type, 2>;  \
    using type##3 = Vector<type, 3>;  \
    using type##4 = Vector<type, 4>;  \
    MAKE_VECTOR_MAKE_TYPE2(type)      \
    MAKE_VECTOR_MAKE_TYPE3(type)      \
    MAKE_VECTOR_MAKE_TYPE4(type)

        MAKE_VECTOR_TYPE(bool)

        MAKE_VECTOR_TYPE(char)

        MAKE_VECTOR_TYPE(uchar)

        MAKE_VECTOR_TYPE(short)

        MAKE_VECTOR_TYPE(ushort)

        MAKE_VECTOR_TYPE(int)

        MAKE_VECTOR_TYPE(uint)

        MAKE_VECTOR_TYPE(float)

        MAKE_VECTOR_TYPE(double)

#undef MAKE_VECTOR_TYPE
#undef MAKE_VECTOR_MAKE_TYPE2
#undef MAKE_VECTOR_MAKE_TYPE3
#undef MAKE_VECTOR_MAKE_TYPE4

// For boolN
        constexpr auto operator!(bool2 v) noexcept { return make_bool2(!v.x, !v.y); }

        constexpr auto operator!(bool3 v) noexcept { return make_bool3(!v.x, !v.y, !v.z); }

        constexpr auto operator!(bool4 v) noexcept { return make_bool4(!v.x, !v.y, !v.z, !v.w); }

        constexpr auto operator||(bool2 lhs, bool2 rhs) noexcept { return make_bool2(lhs.x || rhs.x, lhs.y || rhs.y); }

        constexpr auto operator||(bool3 lhs, bool3 rhs) noexcept {
            return make_bool3(lhs.x || rhs.x, lhs.y || rhs.y, lhs.z || rhs.z);
        }

        constexpr auto operator||(bool4 lhs, bool4 rhs) noexcept {
            return make_bool4(lhs.x || rhs.x, lhs.y || rhs.y, lhs.z || rhs.z, lhs.w || rhs.w);
        }

        constexpr auto operator&&(bool2 lhs, bool2 rhs) noexcept { return make_bool2(lhs.x && rhs.x, lhs.y && rhs.y); }

        constexpr auto operator&&(bool3 lhs, bool3 rhs) noexcept {
            return make_bool3(lhs.x && rhs.x, lhs.y && rhs.y, lhs.z && rhs.z);
        }

        constexpr auto operator&&(bool4 lhs, bool4 rhs) noexcept {
            return make_bool4(lhs.x && rhs.x, lhs.y && rhs.y, lhs.z && rhs.z, lhs.w && rhs.w);
        }

        constexpr bool any(bool2 v) noexcept { return v.x || v.y; }

        constexpr bool any(bool3 v) noexcept { return v.x || v.y || v.z; }

        constexpr bool any(bool4 v) noexcept { return v.x || v.y || v.z || v.w; }

        constexpr bool all(bool2 v) noexcept { return v.x && v.y; }

        constexpr bool all(bool3 v) noexcept { return v.x && v.y && v.z; }

        constexpr bool all(bool4 v) noexcept { return v.x && v.y && v.z && v.w; }

        constexpr bool none(bool2 v) noexcept { return !any(v); }

        constexpr bool none(bool3 v) noexcept { return !any(v); }

        constexpr bool none(bool4 v) noexcept { return !any(v); }

    } // luminous::vector
}// namespace luminous