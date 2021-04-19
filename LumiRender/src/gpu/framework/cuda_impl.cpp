//
// Created by Zero on 2021/4/14.
//

#include "cuda_impl.h"

namespace luminous {
    inline namespace gpu {

        // CUDATexture
        CUDATexture::CUDATexture(PixelFormat pixel_format, uint2 resolution)
                : Impl(pixel_format, resolution) {
            init();
        }

        void CUDATexture::init() {
            CUDA_ARRAY_DESCRIPTOR array_desc{};
            array_desc.Width = _resolution.x;
            array_desc.Height = _resolution.y;
            switch (_pixel_format) {
                case PixelFormat::R8U:
                    array_desc.Format = CU_AD_FORMAT_UNSIGNED_INT8;
                    array_desc.NumChannels = 1;
                    break;
                case PixelFormat::RG8U:
                    array_desc.Format = CU_AD_FORMAT_UNSIGNED_INT8;
                    array_desc.NumChannels = 2;
                    break;
                case PixelFormat::RGBA8U:
                    array_desc.Format = CU_AD_FORMAT_UNSIGNED_INT8;
                    array_desc.NumChannels = 4;
                    break;
                case PixelFormat::R32F:
                    array_desc.Format = CU_AD_FORMAT_FLOAT;
                    array_desc.NumChannels = 1;
                    break;
                case PixelFormat::RG32F:
                    array_desc.Format = CU_AD_FORMAT_FLOAT;
                    array_desc.NumChannels = 2;
                    break;
                case PixelFormat::RGBA32F:
                    array_desc.Format = CU_AD_FORMAT_FLOAT;
                    array_desc.NumChannels = 4;
                    break;
                default:
                    break;
            }

            CUDA_CHECK(cuArrayCreate(&_array_handle, &array_desc));

            CUDA_RESOURCE_DESC res_desc{};
            res_desc.resType = CU_RESOURCE_TYPE_ARRAY;
            res_desc.res.array.hArray = _array_handle;
            res_desc.flags = 0;

            CUDA_TEXTURE_DESC tex_desc{};
            tex_desc.addressMode[0] = CU_TR_ADDRESS_MODE_CLAMP;
            tex_desc.addressMode[1] = CU_TR_ADDRESS_MODE_CLAMP;
            tex_desc.addressMode[2] = CU_TR_ADDRESS_MODE_CLAMP;
            tex_desc.filterMode =  CU_TR_FILTER_MODE_LINEAR;
            tex_desc.flags = CU_TRSF_NORMALIZED_COORDINATES;

            CUDA_RESOURCE_VIEW_DESC res_view_desc{};
            switch (_pixel_format) {
                case PixelFormat::R8U:
                    res_view_desc.format = CU_RES_VIEW_FORMAT_UINT_1X8;
                    break;
                case PixelFormat::RG8U:
                    res_view_desc.format = CU_RES_VIEW_FORMAT_UINT_2X8;
                    break;
                case PixelFormat::RGBA8U:
                    res_view_desc.format = CU_RES_VIEW_FORMAT_UINT_4X8;
                    break;
                case PixelFormat::R32F:
                    res_view_desc.format = CU_RES_VIEW_FORMAT_FLOAT_1X32;
                    break;
                case PixelFormat::RG32F:
                    res_view_desc.format = CU_RES_VIEW_FORMAT_FLOAT_2X32;
                    break;
                case PixelFormat::RGBA32F:
                    res_view_desc.format = CU_RES_VIEW_FORMAT_FLOAT_4X32;
                    break;
                default:
                    break;
            }
            res_view_desc.width = width();
            res_view_desc.height = height();

            CUDA_CHECK(cuTexObjectCreate(&_tex_handle, &res_desc, &tex_desc, &res_view_desc));
            CUDA_CHECK(cuSurfObjectCreate(&_surf_handle, &res_desc));
        }

        CUDATexture::~CUDATexture() {
            CUDA_CHECK(cuArrayDestroy(_array_handle));
            CUDA_CHECK(cuTexObjectDestroy(_tex_handle));
            CUDA_CHECK(cuSurfObjectDestroy(_surf_handle));
        }

        // CUDADispatcher
        CUDADispatcher::CUDADispatcher() {
            CU_CHECK(cuStreamCreate(&stream, CU_STREAM_DEFAULT));
        }

        void CUDADispatcher::wait() {
            CU_CHECK(cuStreamSynchronize(stream));
        }

        void CUDADispatcher::then(std::function<void(void)> F) {
            using Func = std::function<void(void)>;
            Func *f = new Func(std::move(F));
            auto wrapper = [](void *p) {
                auto f = reinterpret_cast<Func *>(p);
                (*f)();
                delete f;
            };
            CU_CHECK(cuLaunchHostFunc(stream, wrapper, (void *) f));
        }

        CUDADispatcher::~CUDADispatcher() {
            CU_CHECK(cuStreamDestroy(stream));
        }

        void *CUDABuffer::ptr() {
            return (void *) _ptr;
        }

        CUDABuffer::CUDABuffer(size_t bytes)
                : _size_in_bytes(bytes) {
            CU_CHECK(cuMemAlloc(&_ptr, bytes));
        }

        CUDABuffer::~CUDABuffer() {
            CU_CHECK(cuMemFree(_ptr));
        }

        size_t CUDABuffer::size() const {
            return _size_in_bytes;
        }

        void *CUDABuffer::address(size_t offset) const {
            return (void *) (_ptr + offset);
        }

        void CUDABuffer::download_async(Dispatcher &dispatcher, void *host_ptr, size_t size, size_t offset) {
            auto stream = dynamic_cast<CUDADispatcher *>(dispatcher.impl_mut())->stream;
            CU_CHECK(cuMemcpyDtoHAsync(host_ptr, _ptr + offset, size, stream));
        }

        void CUDABuffer::upload_async(Dispatcher &dispatcher, const void *host_ptr, size_t size, size_t offset) {
            auto stream = dynamic_cast<CUDADispatcher *>(dispatcher.impl_mut())->stream;
            CU_CHECK(cuMemcpyHtoDAsync(_ptr + offset, host_ptr, size, stream));
        }

        void CUDABuffer::download(void *host_ptr, size_t size, size_t offset) {
            CU_CHECK(cuMemcpyDtoH(host_ptr, _ptr + offset, size));
        }

        void CUDABuffer::upload(const void *host_ptr, size_t size, size_t offset) {
            CU_CHECK(cuMemcpyHtoD(_ptr + offset, host_ptr, size));
        }

        CUDAKernel::CUDAKernel(CUfunction func)
                : _func(func) {
            compute_fit_size();
        }

        void CUDAKernel::compute_fit_size() {
            cuOccupancyMaxPotentialBlockSize(&_min_grid_size, &_auto_block_size, _func, 0, _shared_mem, 0);
        }

        void CUDAKernel::configure(uint3 grid_size, uint3 local_size, size_t sm) {
            _shared_mem = sm == 0 ? _shared_mem : sm;
            _grid_size = grid_size;
            _block_size = local_size;
        }

        void CUDAKernel::launch(Dispatcher &dispatcher, int n_items, std::vector<void *> &args) {
            auto stream = dynamic_cast<CUDADispatcher *>(dispatcher.impl_mut())->stream;
            int grid_size = (n_items + _auto_block_size - 1) / _auto_block_size;
            CU_CHECK(cuLaunchKernel(_func, grid_size, 1, 1,
                                    _auto_block_size, 1, 1,
                                    _shared_mem, stream, args.data(), nullptr));
        }

        void CUDAKernel::launch(Dispatcher &dispatcher, std::vector<void *> &args) {
            auto stream = dynamic_cast<CUDADispatcher *>(dispatcher.impl_mut())->stream;
            CU_CHECK(cuLaunchKernel(_func, _grid_size.x, _grid_size.y, _grid_size.z,
                                    _block_size.x, _block_size.y, _block_size.z,
                                    _shared_mem, stream, args.data(), nullptr));
        }

        CUDADevice::CUDADevice() {
            CU_CHECK(cuInit(0));
            CU_CHECK(cuDeviceGet(&_cu_device, 0));
            CU_CHECK(cuCtxCreate(&_cu_context, 0, _cu_device));
            CU_CHECK(cuCtxSetCurrent(_cu_context));
        }

        RawBuffer CUDADevice::allocate_buffer(size_t bytes) {
            return RawBuffer(std::make_unique<CUDABuffer>(bytes));
        }

        DeviceTexture CUDADevice::allocate_texture(PixelFormat pixel_format, uint2 resolution) {
            return DeviceTexture(nullptr);
        }

        Dispatcher CUDADevice::new_dispatcher() {
            return Dispatcher(std::make_unique<CUDADispatcher>());
        }

        CUDADevice::~CUDADevice() {
            CU_CHECK(cuCtxDestroy(_cu_context));
        }

        CUDAModule::CUDAModule(const std::string &ptx_code) {
            CU_CHECK(cuModuleLoadData(&_module, ptx_code.c_str()));
        }

        SP<Kernel> CUDAModule::get_kernel(const std::string &name) {
            CUfunction func;
            CU_CHECK(cuModuleGetFunction(&func, _module, name.c_str()));
            return create_cuda_kernel(func);
        }

    }
}