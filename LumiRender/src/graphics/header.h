//
// Created by Zero on 2021/2/4.
//


#pragma once



#include <assert.h>

#if defined(__CUDACC__)
    #define XPU __host__ __device__
    #define GPU __device__
    #define CPU __host__
#else
    #define XPU
    #define GPU
    #define CPU
#endif

#if defined(__CUDA_ARCH__)
    #define IS_GPU_CODE
#endif

#define GEN_CLASS_NAME(...)  XPU static constexpr const char *name() { return #__VA_ARGS__; }

#define GEN_BASE_NAME(arg)  XPU static constexpr const char *base_name() { return #arg; }

#define GEN_NAME_FUNC NDSC_XPU const char *name() {             \
                                    LUMINOUS_VAR_DISPATCH(name);\
                               }

#ifdef IS_GPU_CODE
#define GEN_TO_STRING_FUNC NDSC std::string to_string() const { \
                LUMINOUS_ERROR("device not support to string")  \
        }
#else
#define GEN_TO_STRING_FUNC NDSC std::string to_string() const { \
                LUMINOUS_VAR_DISPATCH(to_string);               \
        }
#endif

#define GEN_NAME_AND_TO_STRING_FUNC GEN_NAME_FUNC \
                                    GEN_TO_STRING_FUNC
#define XPU_INLINE XPU __forceinline

#define GPU_INLINE GPU __forceinline

#define NDSC [[nodiscard]]

#define NDSC_XPU NDSC XPU
#define NDSC_XPU_INLINE NDSC XPU_INLINE



#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

#define USE_LSTD 1

#define DCHECK(a) assert(a);
#define DCHECK_EQ(a, b) DCHECK(a == b)
#define DCHECK_GT(a, b) DCHECK(a > b);
#define DCHECK_GE(a, b) DCHECK(a >= b);
#define DCHECK_LT(a, b) DCHECK(a < b);
#define DCHECK_LE(a, b) DCHECK(a <= b);

