//
// Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once
#include <cuda_runtime.h>
#include <optix.h>
#include <optix_function_table_definition.h>
#include <optix_stubs.h>
#include "gpu/framework/helper/cuda.h"
#include "gpu/framework/helper/optix.h"
#include <optix_stack_size.h>

#include "optixPathTracer.h"

#include <array>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "render/sensors/sensor.h"
#include "util/clock.h"
#include "graphics/geometry/transform.h"

bool resize_dirty = false;
bool minimized    = false;

// Camera state
bool             camera_changed = true;

luminous::Sensor camera;

// Mouse state
int32_t mouse_button = -1;

int32_t samples_per_launch = 1;

//------------------------------------------------------------------------------
//
// Local types
// TODO: some of these should move to sutil or optix util header
//
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//
// Scene data
//
//------------------------------------------------------------------------------

const int32_t TRIANGLE_COUNT = 32;
const int32_t MAT_COUNT      = 1;

const static std::vector<luminous::float3> g_vertices =
        {
                   // Floor  -- white lambert
                   luminous::float3(    0.0f,    0.0f,    0.0f),
                   luminous::float3(    0.0f,    0.0f,  559.2f),
                   luminous::float3(  556.0f,    0.0f,  559.2f),
                   luminous::float3(    0.0f,    0.0f,    0.0f),
                   luminous::float3(  556.0f,    0.0f,  559.2f),
                   luminous::float3(  556.0f,    0.0f,    0.0f),

                   // Ceiling -- white lambert
                   luminous::float3(    0.0f,  548.8f,    0.0f),
                   luminous::float3(  556.0f,  548.8f,    0.0f),
                   luminous::float3(  556.0f,  548.8f,  559.2f),

                   luminous::float3(    0.0f,  548.8f,    0.0f),
                   luminous::float3(  556.0f,  548.8f,  559.2f),
                   luminous::float3(    0.0f,  548.8f,  559.2f),

                   // Back wall -- white lambert
                   luminous::float3(    0.0f,    0.0f,  559.2f),
                   luminous::float3(    0.0f,  548.8f,  559.2f),
                   luminous::float3(  556.0f,  548.8f,  559.2f),

                   luminous::float3(    0.0f,    0.0f,  559.2f),
                   luminous::float3(  556.0f,  548.8f,  559.2f),
                   luminous::float3(  556.0f,    0.0f,  559.2f),

                   // Right wall -- green lambert
                   luminous::float3(    0.0f,    0.0f,    0.0f),
                   luminous::float3(    0.0f,  548.8f,    0.0f),
                   luminous::float3(    0.0f,  548.8f,  559.2f),

                   luminous::float3(    0.0f,    0.0f,    0.0f),
                   luminous::float3(    0.0f,  548.8f,  559.2f),
                   luminous::float3(    0.0f,    0.0f,  559.2f),

                   // Left wall -- red lambert
                   luminous::float3(  556.0f,    0.0f,    0.0f),
                   luminous::float3(  556.0f,    0.0f,  559.2f),
                   luminous::float3(  556.0f,  548.8f,  559.2f),

                   luminous::float3(  556.0f,    0.0f,    0.0f),
                   luminous::float3(  556.0f,  548.8f,  559.2f),
                   luminous::float3(  556.0f,  548.8f,    0.0f),

                   // Short block -- white lambert
                   luminous::float3(  130.0f,  165.0f,   65.0f),
                   luminous::float3(   82.0f,  165.0f,  225.0f),
                   luminous::float3(  242.0f,  165.0f,  274.0f),

                   luminous::float3(  130.0f,  165.0f,   65.0f),
                   luminous::float3(  242.0f,  165.0f,  274.0f),
                   luminous::float3(  290.0f,  165.0f,  114.0f),

                   luminous::float3(  290.0f,    0.0f,  114.0f),
                   luminous::float3(  290.0f,  165.0f,  114.0f),
                   luminous::float3(  240.0f,  165.0f,  272.0f),

                   luminous::float3(  290.0f,    0.0f,  114.0f),
                   luminous::float3(  240.0f,  165.0f,  272.0f),
                   luminous::float3(  240.0f,    0.0f,  272.0f),

                   luminous::float3(  130.0f,    0.0f,   65.0f),
                   luminous::float3(  130.0f,  165.0f,   65.0f),
                   luminous::float3(  290.0f,  165.0f,  114.0f),

                   luminous::float3(  130.0f,    0.0f,   65.0f),
                   luminous::float3(  290.0f,  165.0f,  114.0f),
                   luminous::float3(  290.0f,    0.0f,  114.0f),

                   luminous::float3(   82.0f,    0.0f,  225.0f),
                   luminous::float3(   82.0f,  165.0f,  225.0f),
                   luminous::float3(  130.0f,  165.0f,   65.0f),

                   luminous::float3(   82.0f,    0.0f,  225.0f),
                   luminous::float3(  130.0f,  165.0f,   65.0f),
                   luminous::float3(  130.0f,    0.0f,   65.0f),

                   luminous::float3(  240.0f,    0.0f,  272.0f),
                   luminous::float3(  240.0f,  165.0f,  272.0f),
                   luminous::float3(   82.0f,  165.0f,  225.0f),

                   luminous::float3(  240.0f,    0.0f,  272.0f),
                   luminous::float3(   82.0f,  165.0f,  225.0f),
                   luminous::float3(   82.0f,    0.0f,  225.0f),

                   // Tall block -- white lambert
                   luminous::float3(  423.0f,  330.0f,  247.0f),
                   luminous::float3(  265.0f,  330.0f,  296.0f),
                   luminous::float3(  314.0f,  330.0f,  455.0f),

                   luminous::float3(  423.0f,  330.0f,  247.0f),
                   luminous::float3(  314.0f,  330.0f,  455.0f),
                   luminous::float3(  472.0f,  330.0f,  406.0f),

                   luminous::float3(  423.0f,    0.0f,  247.0f),
                   luminous::float3(  423.0f,  330.0f,  247.0f),
                   luminous::float3(  472.0f,  330.0f,  406.0f),

                   luminous::float3(  423.0f,    0.0f,  247.0f),
                   luminous::float3(  472.0f,  330.0f,  406.0f),
                   luminous::float3(  472.0f,    0.0f,  406.0f),

                   luminous::float3(  472.0f,    0.0f,  406.0f),
                   luminous::float3(  472.0f,  330.0f,  406.0f),
                   luminous::float3(  314.0f,  330.0f,  456.0f),

                   luminous::float3(  472.0f,    0.0f,  406.0f),
                   luminous::float3(  314.0f,  330.0f,  456.0f),
                   luminous::float3(  314.0f,    0.0f,  456.0f),

                   luminous::float3(  314.0f,    0.0f,  456.0f),
                   luminous::float3(  314.0f,  330.0f,  456.0f),
                   luminous::float3(  265.0f,  330.0f,  296.0f),

                   luminous::float3(  314.0f,    0.0f,  456.0f),
                   luminous::float3(  265.0f,  330.0f,  296.0f),
                   luminous::float3(  265.0f,    0.0f,  296.0f),

                   luminous::float3(  265.0f,    0.0f,  296.0f),
                   luminous::float3(  265.0f,  330.0f,  296.0f),
                   luminous::float3(  423.0f,  330.0f,  247.0f),

                   luminous::float3(  265.0f,    0.0f,  296.0f),
                   luminous::float3(  423.0f,  330.0f,  247.0f),
                   luminous::float3(  423.0f,    0.0f,  247.0f),

                   // Ceiling light -- emmissive
                   luminous::float3(  343.0f,  548.6f,  227.0f),
                   luminous::float3(  213.0f,  548.6f,  227.0f),
                   luminous::float3(  213.0f,  548.6f,  332.0f),

                   luminous::float3(  343.0f,  548.6f,  227.0f),
                   luminous::float3(  213.0f,  548.6f,  332.0f),
                   luminous::float3(  343.0f,  548.6f,  332.0f)
            };

std::vector<luminous::TriangleHandle> tri_list;

void initLaunchParams( PathTracerState& state )
{
    state.params.traversable_handle         = state.gas_handle;

    CUDA_CHECK( cudaStreamCreate( &state.stream ) );

    state.b_params = state.device->allocate_buffer<Params>(1);
}


void handleCameraUpdate( Params& params )
{

}



void launchSubframe(PathTracerState& state )
{
    state.b_params.upload_async(state.dispatcher,&state.params);

    OPTIX_CHECK( optixLaunch(
            state.pipeline,
            state.stream,
            state.b_params.ptr<CUdeviceptr>(),
            sizeof( Params ),
            &state.sbt,
            768,   // launch width
            768,  // launch height
            1                     // launch depth
    ) );
    cudaDeviceSynchronize();
}

static void context_log_cb( unsigned int level, const char* tag, const char* message, void* /*cbdata */ )
{
    std::cerr << "[" << std::setw( 2 ) << level << "][" << std::setw( 12 ) << tag << "]: " << message << "\n";
}


void initCameraState()
{
    using namespace luminous;
    SensorConfig sc;
    sc.set_full_type("PinholeCamera");
    sc.fov_y = 35;
    sc.transform_config.position = luminous::make_float3(278.0f, 273.0f, -900.0f);
    sc.transform_config.yaw = 180;
    sc.transform_config.set_type("yaw_pitch");
    sc.film_config.set_full_type("RGBFilm");
    sc.film_config.resolution = luminous::make_uint2(768);
    camera = Sensor::create(sc);
}


void createContext( PathTracerState& state )
{
    // Initialize CUDA
    CU_CHECK( cuMemFree( 0 ) );

    OptixDeviceContext context;
    CUcontext          cu_ctx = 0;  // zero means take the current context
    OPTIX_CHECK( optixInit() );
    OptixDeviceContextOptions options = {};
    options.logCallbackFunction       = &context_log_cb;
    options.logCallbackLevel          = 4;
    OPTIX_CHECK( optixDeviceContextCreate( cu_ctx, &options, &context ) );

    state.context = context;
}

using namespace std;

void buildInstanceAccel(PathTracerState &state) {

    vector<OptixTraversableHandle> traversable_handles;
    traversable_handles.push_back(state.gas_handle);

    OptixBuildInput instance_input = {};
    state.instances = state.device->allocate_buffer<OptixInstance>(1);
    instance_input.type = OPTIX_BUILD_INPUT_TYPE_INSTANCES;
    instance_input.instanceArray.numInstances = 1;
    instance_input.instanceArray.instances = state.instances.ptr<CUdeviceptr>();
    OptixAccelBuildOptions accel_options = {};
    accel_options.buildFlags = (OPTIX_BUILD_FLAG_ALLOW_COMPACTION | OPTIX_BUILD_FLAG_PREFER_FAST_TRACE);
    accel_options.operation = OPTIX_BUILD_OPERATION_BUILD;

    OptixAccelBufferSizes ias_buffer_sizes;
    OPTIX_CHECK(optixAccelComputeMemoryUsage(state.context,
                                             &accel_options, &instance_input,
                                             1,  // num build inputs
                                             &ias_buffer_sizes));

    state.ias_buffer = state.device->allocate_buffer(ias_buffer_sizes.outputSizeInBytes);
    auto temp_buffer = state.device->allocate_buffer(ias_buffer_sizes.tempSizeInBytes);
    auto compact_size_buffer = state.device->allocate_buffer<uint64_t>(1);
    OptixAccelEmitDesc emit_desc;
    emit_desc.type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE;
    emit_desc.result = compact_size_buffer.ptr<uint64_t>();

    vector<OptixInstance> optix_instances;
    optix_instances.reserve(1);
    for (int i = 0; i < 1; ++i) {
        auto transform = luminous::Transform(luminous::make_float4x4());
        OptixInstance optix_instance;
        optix_instance.traversableHandle = traversable_handles[0];
        optix_instance.flags = OPTIX_INSTANCE_FLAG_NONE;
        optix_instance.instanceId = i;
        optix_instance.visibilityMask = 1;
        optix_instance.sbtOffset = 0;
        luminous::mat4x4_to_array12(transform.mat4x4(), optix_instance.transform);
        optix_instances.push_back(optix_instance);
    }

    state.instances.upload(optix_instances.data());

    OPTIX_CHECK(optixAccelBuild(state.context,
                                0, &accel_options,
                                &instance_input, 1,
                                temp_buffer.ptr<CUdeviceptr>(),
                                ias_buffer_sizes.tempSizeInBytes,
                                state.ias_buffer.ptr<CUdeviceptr>(),
                                ias_buffer_sizes.outputSizeInBytes,
                                &state.ias_handle, &emit_desc, 1));
    CU_CHECK(cuCtxSynchronize());
}

void buildMeshAccel( PathTracerState& state )
{
    //
    // copy mesh data to device
    //

    using namespace luminous;
    state.pos_buffer = state.device->allocate_buffer<luminous::float3>(g_vertices.size());
    state.pos_buffer.upload(g_vertices.data());
    state.d_vertices = state.pos_buffer.ptr<CUdeviceptr>();

    state.index_buffer = state.device->allocate_buffer<luminous::TriangleHandle>(tri_list.size());
    state.index_buffer.upload(tri_list.data());
    state.d_tri = state.index_buffer.ptr<CUdeviceptr>();
    //
    // Build triangle GAS
    //
    uint32_t triangle_input_flags[MAT_COUNT] =  // One per SBT record for this build input
            {
                    OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT
            };

    OptixBuildInput triangle_input                           = {};
    triangle_input.type                                      = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
    triangle_input.triangleArray.vertexFormat                = OPTIX_VERTEX_FORMAT_FLOAT3;
    triangle_input.triangleArray.vertexStrideInBytes         = sizeof( luminous::float3 );
    triangle_input.triangleArray.numVertices                 = static_cast<uint32_t>( g_vertices.size() );
    triangle_input.triangleArray.vertexBuffers               = &state.d_vertices;
    triangle_input.triangleArray.flags                       = triangle_input_flags;
    triangle_input.triangleArray.numSbtRecords               = MAT_COUNT;
    triangle_input.triangleArray.indexBuffer = state.d_tri;
    triangle_input.triangleArray.indexStrideInBytes = sizeof(luminous::TriangleHandle);
    triangle_input.triangleArray.indexFormat = OPTIX_INDICES_FORMAT_UNSIGNED_INT3;
    triangle_input.triangleArray.numIndexTriplets = tri_list.size();

    OptixAccelBuildOptions accel_options = {};
    accel_options.buildFlags             = OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
    accel_options.operation              = OPTIX_BUILD_OPERATION_BUILD;

    OptixAccelBufferSizes gas_buffer_sizes;
    OPTIX_CHECK( optixAccelComputeMemoryUsage(
            state.context,
            &accel_options,
            &triangle_input,
            1,  // num_build_inputs
            &gas_buffer_sizes
    ) );

    auto temp_buffer = state.device->allocate_buffer(gas_buffer_sizes.tempSizeInBytes);
    state.gas_buffer = state.device->allocate_buffer(gas_buffer_sizes.outputSizeInBytes);

    auto compact_size_buffer = state.device->allocate_buffer<uint64_t>(1);

    OptixAccelEmitDesc emitProperty = {};
    emitProperty.type               = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE;
    emitProperty.result = compact_size_buffer.ptr<CUdeviceptr>();


    OPTIX_CHECK( optixAccelBuild(
            state.context,
            0,                                  // CUDA stream
            &accel_options,
            &triangle_input,
            1,                                  // num build inputs
            temp_buffer.ptr<CUdeviceptr>(),
            gas_buffer_sizes.tempSizeInBytes,
            state.gas_buffer.ptr<CUdeviceptr>(),
            gas_buffer_sizes.outputSizeInBytes,
            &state.gas_handle,
            &emitProperty,                      // emitted property list
            1                                   // num emitted properties
    ) );
    CU_CHECK(cuCtxSynchronize());
}


void createModule( PathTracerState& state )
{
    OptixModuleCompileOptions module_compile_options = {};
    module_compile_options.maxRegisterCount  = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
    module_compile_options.optLevel          = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
    module_compile_options.debugLevel        = OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO;

    state.pipeline_compile_options.usesMotionBlur        = false;
    state.pipeline_compile_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
    state.pipeline_compile_options.numPayloadValues      = 2;
    state.pipeline_compile_options.numAttributeValues    = 2;
#ifdef DEBUG // Enables debug exceptions during optix launches. This may incur significant performance cost and should only be done during development.
    state.pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_DEBUG | OPTIX_EXCEPTION_FLAG_TRACE_DEPTH | OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW;
#else
    state.pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;
#endif
    state.pipeline_compile_options.pipelineLaunchParamsVariableName = "params";

    const std::string ptx(sdk_ptx);

    char   log[2048];
    size_t sizeof_log = sizeof( log );
    OPTIX_CHECK( optixModuleCreateFromPTX(
            state.context,
            &module_compile_options,
            &state.pipeline_compile_options,
            ptx.c_str(),
            ptx.size(),
            log,
            &sizeof_log,
            &state.ptx_module
    ) );
}


void createProgramGroups( PathTracerState& state )
{
    OptixProgramGroupOptions  program_group_options = {};

    char   log[2048];
    size_t sizeof_log = sizeof( log );

    {
        OptixProgramGroupDesc raygen_prog_group_desc    = {};
        raygen_prog_group_desc.kind                     = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
        raygen_prog_group_desc.raygen.module            = state.ptx_module;
        raygen_prog_group_desc.raygen.entryFunctionName = "__raygen__rg";

        OPTIX_CHECK( optixProgramGroupCreate(
                state.context, &raygen_prog_group_desc,
                1,  // num program groups
                &program_group_options,
                log,
                &sizeof_log,
                &state.raygen_prog_group
        ) );
    }

    {
        OptixProgramGroupDesc miss_prog_group_desc  = {};
        miss_prog_group_desc.kind                   = OPTIX_PROGRAM_GROUP_KIND_MISS;
        miss_prog_group_desc.miss.module            = state.ptx_module;
        miss_prog_group_desc.miss.entryFunctionName = "__miss__radiance";
        sizeof_log                                  = sizeof( log );
        OPTIX_CHECK( optixProgramGroupCreate(
                state.context, &miss_prog_group_desc,
                1,  // num program groups
                &program_group_options,
                log, &sizeof_log,
                &state.radiance_miss_group
        ) );

        memset( &miss_prog_group_desc, 0, sizeof( OptixProgramGroupDesc ) );
        miss_prog_group_desc.kind                   = OPTIX_PROGRAM_GROUP_KIND_MISS;
        miss_prog_group_desc.miss.module            = nullptr;  // NULL miss program for occlusion rays
        miss_prog_group_desc.miss.entryFunctionName = nullptr;
        sizeof_log                                  = sizeof( log );
        OPTIX_CHECK( optixProgramGroupCreate(
                state.context, &miss_prog_group_desc,
                1,  // num program groups
                &program_group_options,
                log,
                &sizeof_log,
                &state.occlusion_miss_group
        ) );
    }

    {
        OptixProgramGroupDesc hit_prog_group_desc        = {};
        hit_prog_group_desc.kind                         = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
        hit_prog_group_desc.hitgroup.moduleCH            = state.ptx_module;
        hit_prog_group_desc.hitgroup.entryFunctionNameCH = "__closesthit__radiance";
        sizeof_log                                       = sizeof( log );
        OPTIX_CHECK( optixProgramGroupCreate(
                state.context,
                &hit_prog_group_desc,
                1,  // num program groups
                &program_group_options,
                log,
                &sizeof_log,
                &state.radiance_hit_group
        ) );

        memset( &hit_prog_group_desc, 0, sizeof( OptixProgramGroupDesc ) );
        hit_prog_group_desc.kind                         = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
        hit_prog_group_desc.hitgroup.moduleCH            = state.ptx_module;
        hit_prog_group_desc.hitgroup.entryFunctionNameCH = "__closesthit__occlusion";
        sizeof_log                                       = sizeof( log );
        OPTIX_CHECK( optixProgramGroupCreate(
                state.context,
                &hit_prog_group_desc,
                1,  // num program groups
                &program_group_options,
                log,
                &sizeof_log,
                &state.occlusion_hit_group
        ) );
    }
}


void createPipeline( PathTracerState& state )
{
    OptixProgramGroup program_groups[] =
            {
                    state.raygen_prog_group,
                    state.radiance_miss_group,
                    state.occlusion_miss_group,
                    state.radiance_hit_group,
                    state.occlusion_hit_group
            };

    OptixPipelineLinkOptions pipeline_link_options = {};
    pipeline_link_options.maxTraceDepth            = 2;
    pipeline_link_options.debugLevel               = OPTIX_COMPILE_DEBUG_LEVEL_FULL;

    char   log[2048];
    size_t sizeof_log = sizeof( log );
    OPTIX_CHECK( optixPipelineCreate(
            state.context,
            &state.pipeline_compile_options,
            &pipeline_link_options,
            program_groups,
            sizeof( program_groups ) / sizeof( program_groups[0] ),
            log,
            &sizeof_log,
            &state.pipeline
    ) );

    // We need to specify the max traversal depth.  Calculate the stack sizes, so we can specify all
    // parameters to optixPipelineSetStackSize.
    OptixStackSizes stack_sizes = {};
    OPTIX_CHECK( optixUtilAccumulateStackSizes( state.raygen_prog_group,    &stack_sizes ) );
    OPTIX_CHECK( optixUtilAccumulateStackSizes( state.radiance_miss_group,  &stack_sizes ) );
    OPTIX_CHECK( optixUtilAccumulateStackSizes( state.occlusion_miss_group, &stack_sizes ) );
    OPTIX_CHECK( optixUtilAccumulateStackSizes( state.radiance_hit_group,   &stack_sizes ) );
    OPTIX_CHECK( optixUtilAccumulateStackSizes( state.occlusion_hit_group,  &stack_sizes ) );

    uint32_t max_trace_depth = 2;
    uint32_t max_cc_depth = 0;
    uint32_t max_dc_depth = 0;
    uint32_t direct_callable_stack_size_from_traversal;
    uint32_t direct_callable_stack_size_from_state;
    uint32_t continuation_stack_size;
    OPTIX_CHECK( optixUtilComputeStackSizes(
            &stack_sizes,
            max_trace_depth,
            max_cc_depth,
            max_dc_depth,
            &direct_callable_stack_size_from_traversal,
            &direct_callable_stack_size_from_state,
            &continuation_stack_size
    ) );

    const uint32_t max_traversal_depth = 1;
    OPTIX_CHECK( optixPipelineSetStackSize(
            state.pipeline,
            direct_callable_stack_size_from_traversal,
            direct_callable_stack_size_from_state,
            continuation_stack_size,
            max_traversal_depth
    ) );
}


void createSBT( PathTracerState& state )
{
    state.rg_buffer = state.device->allocate_buffer<RayGenRecord>(1);

    RayGenRecord rg_sbt = {};
    OPTIX_CHECK( optixSbtRecordPackHeader( state.raygen_prog_group, &rg_sbt ) );
    state.rg_buffer.upload(&rg_sbt);

    const size_t miss_record_size = sizeof( MissRecord );
    state.ms_rcd_buffer = state.device->allocate_buffer<MissRecord>(RAY_TYPE_COUNT);
    MissRecord ms_sbt[2];
    OPTIX_CHECK( optixSbtRecordPackHeader( state.radiance_miss_group,  &ms_sbt[0] ) );
    ms_sbt[0].data.bg_color = luminous::make_float4( 0.0f );
    OPTIX_CHECK( optixSbtRecordPackHeader( state.occlusion_miss_group, &ms_sbt[1] ) );
    ms_sbt[1].data.bg_color = luminous::make_float4( 0.0f );
    state.ms_rcd_buffer.upload(ms_sbt);


    state.hg_rcd_buffer = state.device->allocate_buffer<HitGroupRecord>(RAY_TYPE_COUNT);

    const size_t hitgroup_record_size = sizeof( HitGroupRecord );

    HitGroupRecord hitgroup_records[RAY_TYPE_COUNT * MAT_COUNT];
    {
        const int sbt_idx = 0;  // SBT for radiance ray-type for ith material

        OPTIX_CHECK( optixSbtRecordPackHeader( state.radiance_hit_group, &hitgroup_records[sbt_idx] ) );
        hitgroup_records[sbt_idx].data.vertices       = reinterpret_cast<luminous::float4*>( state.d_vertices );
    }

    {
        const int sbt_idx = 1;  // SBT for occlusion ray-type for ith material
        memset( &hitgroup_records[sbt_idx], 0, hitgroup_record_size );

        OPTIX_CHECK( optixSbtRecordPackHeader( state.occlusion_hit_group, &hitgroup_records[sbt_idx] ) );
    }

    state.hg_rcd_buffer.upload(hitgroup_records);


    state.sbt.raygenRecord                = state.rg_buffer.ptr<CUdeviceptr>();
    state.sbt.missRecordBase              = state.ms_rcd_buffer.ptr<CUdeviceptr>();
    state.sbt.missRecordStrideInBytes     = static_cast<uint32_t>( miss_record_size );
    state.sbt.missRecordCount             = RAY_TYPE_COUNT;
    state.sbt.hitgroupRecordBase          = state.hg_rcd_buffer.ptr<CUdeviceptr>();
    state.sbt.hitgroupRecordStrideInBytes = static_cast<uint32_t>( hitgroup_record_size );
    state.sbt.hitgroupRecordCount         = RAY_TYPE_COUNT * MAT_COUNT;
}

void init_tri_list() {
    for (int i = 0; i < g_vertices.size() / 3; ++i) {
        uint32_t s = 3 * i;
        tri_list.push_back(luminous::TriangleHandle{s, s + 1, s + 2});
    }
}

void init(PathTracerState &state) {
    initCameraState();
    init_tri_list();
    createContext( state );
    buildMeshAccel( state );
//        buildInstanceAccel(state);
    createModule( state );
    createProgramGroups( state );
    createPipeline( state );
    createSBT( state );
    initLaunchParams( state );
}

int run() {
    PathTracerState state;

    init(state);

    luminous::Clock clk;
    int i = 0;
    double t = 0;
    while (1) {
        clk.start();
        launchSubframe(state);
        double dt = clk.elapse_ms();
        t += dt;
        cout << "dt = "<< dt <<",count = " << ++i <<",average = " << t / i << endl;
    }
    return 0;
}
