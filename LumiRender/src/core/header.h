//
// Created by Zero on 2020/8/31.
//

#pragma once

#include "macro.h"
#include "ext/nlohmann/json.hpp"
#include "logging.h"

#if defined(_MSC_VER)
#define HAVE_ALIGNED_MALLOC
#endif

#ifndef L1_CACHE_LINE_SIZE
#define L1_CACHE_LINE_SIZE 64
#endif

#define HAVE_POSIX_MEMALIGN

#define XPU __host__ __device__
#define GPU __device__
#define CPU __host__
using DataWrap = nlohmann::json ;
