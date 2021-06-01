# ======================================================================== #
# Copyright 2018-2020 Ingo Wald                                            #
#                                                                          #
# Licensed under the Apache License, Version 2.0 (the "License");          #
# you may not use this file except in compliance with the License.         #
# You may obtain a copy of the License at                                  #
#                                                                          #
#     http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                          #
# Unless required by applicable law or agreed to in writing, software      #
# distributed under the License is distributed on an "AS IS" BASIS,        #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
# See the License for the specific language governing permissions and      #
# limitations under the License.                                           #
# ======================================================================== #

#set(CMAKE_MODULE_PATH
#  "${CMAKE_CURRENT_SOURCE_DIR}/../cmake"
#  ${CMAKE_MODULE_PATH}
#  )

include(configure_cuda)
find_package(CUDA REQUIRED)
find_package(OptiX REQUIRED VERSION 7)

include_directories(${CUDA_TOOLKIT_INCLUDE})
include_directories(${OptiX_INCLUDE})

if (WIN32)
    add_definitions(-DNOMINMAX)
endif ()


find_program(BIN2C bin2c
        DOC "Path to the cuda-sdk bin2c executable.")

# this macro defines cmake rules that execute the following four steps:
# 1) compile the given cuda file ${cuda_file} to an intermediary PTX file
# 2) use the 'bin2c' tool (that comes with CUDA) to
#    create a second intermediary (.c-)file which defines a const string variable
#    (named '${c_var_name}') whose (constant) value is the PTX output
#    from the previous step.
# 3) compile the given .c file to an intermediary object file (why thus has
#    that PTX string 'embedded' as a global constant.
# 4) assign the name of the intermediary .o file to the cmake variable
#    'output_var', which can then be added to cmake targets.
macro(cuda_compile_and_embed output_var cuda_file)
    set(c_var_name ${output_var})
    if (${CMAKE_BUILD_TYPE} MATCHES "Release")
        cuda_compile_ptx(ptx_files
                ${cuda_file}
                OPTIONS -O3 -D NDEBUG=1 -D _ENABLE_EXTENDED_ALIGNED_STORAGE
                -use_fast_math
                --expt-relaxed-constexpr
                --std=c++17
                -arch compute_60
                -D__x86_64
                )
    else ()
        cuda_compile_ptx(ptx_files
                ${cuda_file}
                OPTIONS -D _ENABLE_EXTENDED_ALIGNED_STORAGE --expt-relaxed-constexpr --std=c++17
                )
    endif ()
    list(GET ptx_files 0 ptx_file)
    set(embedded_file ${ptx_file}_embedded.c)
    message("adding rule to compile and embed ${cuda_file} to \"const char ${output_var}[];\"")
    add_custom_command(
            OUTPUT ${embedded_file}
            COMMAND ${BIN2C} -c --padd 0 --type char --name ${c_var_name} ${ptx_file} > ${embedded_file}
            DEPENDS ${ptx_file}
            COMMENT "compiling (and embedding ptx from) ${cuda_file}"
    )
    set(${output_var} ${embedded_file})
endmacro()


