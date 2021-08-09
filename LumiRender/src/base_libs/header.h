//
// Created by Zero on 2021/2/4.
//


#pragma once


#include <typeinfo>
#include <assert.h>

#if defined(_MSC_VER)
#define HAVE_ALIGNED_MALLOC
#endif

#ifndef L1_CACHE_LINE_SIZE
#define L1_CACHE_LINE_SIZE 64
#endif

#define F_INLINE __forceinline

#define INLINE inline

#define XPU_INLINE XPU INLINE

#define GPU_INLINE GPU INLINE

#define NDSC [[nodiscard]]

#define NDSC_XPU NDSC XPU
#define NDSC_INLINE NDSC INLINE
#define NDSC_XPU_INLINE NDSC XPU_INLINE

#define TO_STRING(x) TO_STRING2(x)
#define TO_STRING2(x) #x

#if defined(__CUDACC__)
#define IS_GPU_CODE
#endif

#define HAVE_POSIX_MEMALIGN

#ifdef IS_GPU_CODE

    #define LUMINOUS_DBG(...) printf(__FILE__ ":" TO_STRING(__LINE__) ": " __VA_ARGS__)
    #define XPU __host__ __device__
    #define GPU __device__
    #define CPU __host__

    #define CPU_ONLY(arg)

    #define GEN_NAME_FUNC NDSC_XPU const char *name() {             \
                                        LUMINOUS_VAR_DISPATCH(name);\
                                   }

    #define GEN_STRING_FUNC(args)

#else

    #define XPU
    #define GPU
    #define CPU
    #define LUMINOUS_DBG(...) fprintf(stderr, __FILE__ ":" TO_STRING(__LINE__) ": " __VA_ARGS__)

    #define CPU_ONLY(arg) arg

    #define GEN_STRING_FUNC(args) [[nodiscard]] std::string to_string() const args

    #define GEN_NAME_FUNC NDSC_XPU const std::string name() {       \
                                        return this->dispatch([&, this](auto &&self) { return type_name(&self); });\
                                   }
#endif

#define LUMINOUS_TO_STRING(...) return string_printf(__VA_ARGS__);

template<typename T>
constexpr const char * type_name(T * ptr = nullptr) {
    if (ptr == nullptr)
        return typeid(T).name();
    else
        return typeid(*ptr).name();
}

#define GEN_BASE_NAME(arg) XPU static constexpr const char *base_name() { return #arg; }

#define GEN_TO_STRING_FUNC GEN_STRING_FUNC({LUMINOUS_VAR_DISPATCH(to_string);})

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

#define CONTINUE_IF(condition) if((condition)) { continue; }
#define CONTINUE_IF_TIPS(condition, str) if((condition)) { LUMINOUS_DBG(str); continue; }

#define BREAK_IF(condition) if((condition)) { break; }
#define BREAK_IF_TIPS(condition, str) if((condition)) { LUMINOUS_DBG(str); break; }
