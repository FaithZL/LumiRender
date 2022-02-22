//
// Created by Zero on 2021/5/1.
//


#pragma once

#if defined(__CUDACC__)
    #include "material.cpp"
    #include "matte.cpp"
    #include "glass.cpp"
    #include "metal.cpp"
    #include "mirror.cpp"
    #include "disney.cpp"
    #include "plastic.cpp"
    #include "ai_material.cpp"
#else
    #include "common.h"
#endif