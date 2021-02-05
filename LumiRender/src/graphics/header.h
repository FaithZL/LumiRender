//
// Created by Zero on 2021/2/4.
//


#pragma once

#define XPU
#define GPU
#define CPU

#include <assert.h>

#if defined(__CUDACC__)
    #define XPU __host__ __device__
    #define GPU __device__
    #define CPU __host__
#endif

#if defined(__CUDA_ARCH__)
    #define IS_GPU_CODE
#endif