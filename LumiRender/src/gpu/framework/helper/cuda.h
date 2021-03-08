//
// Created by Zero on 2021/2/12.
//


#pragma once

#include "graphics/header.h"
#include <cuda_runtime.h>
#include <stdexcept>
#include "spdlog/spdlog.h"

#define CUDA_CHECK(EXPR)                                                                                               \
    [&] {                                                                                                              \
        if (EXPR != cudaSuccess) {                                                                                     \
            cudaError_t error = cudaGetLastError();                                                                    \
            spdlog::error("CUDA error: {} at {}:{}", cudaGetErrorString(error), __FILE__, __LINE__);                   \
            std::abort();                                                                                              \
        }                                                                                                              \
    }()

#define CU_CHECK(EXPR)                                                                                                 \
    [&] {                                                                                                              \
        CUresult result = EXPR;                                                                                        \
        if (result != CUDA_SUCCESS) {                                                                                  \
            const char *str;                                                                                           \
            assert(CUDA_SUCCESS == cuGetErrorString(result, &str));                                                \
            spdlog::error("CUDA error: {} at {}:{}", str, __FILE__, __LINE__);                                         \
            std::abort();                                                                                              \
        }                                                                                                              \
    }()