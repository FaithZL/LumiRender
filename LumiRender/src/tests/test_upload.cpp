//
// Created by Zero on 27/09/2021.
//

#include "core/refl/factory.h"
#include "render/samplers/sampler.h"
#include "core/backend/managed.h"
#include "cpu/cpu_impl.h"
#include "core/refl/reflection.h"
#include "core/backend/ptr_mapper.h"
#include "core/backend/synchronizer.h"

using namespace std;
using namespace luminous;

using lstd::Variant;


class TestSampler : BASE_CLASS(Variant<LCGSampler *, PCGSampler *>) {
public:
    REFL_CLASS(TestSampler)

private:
    using BaseBinder::BaseBinder;
public:
    GEN_BASE_NAME(TestSampler)

    LM_NODISCARD XPU int spp() const {
        LUMINOUS_VAR_PTR_DISPATCH(spp)
    }

    XPU void start_pixel_sample(uint2 pixel, int sample_index, int dimension) {
        LUMINOUS_VAR_PTR_DISPATCH(start_pixel_sample, pixel, sample_index, dimension)
    }

    XPU SensorSample sensor_sample(uint2 p_raster) {
        SensorSample ss;
        ss.p_film = make_float2(p_raster) + next_2d();
        ss.p_lens = next_2d();
        ss.time = next_1d();
        return ss;
    }

    NDSC_XPU float next_1d() {
        LUMINOUS_VAR_PTR_DISPATCH(next_1d)
    }

    NDSC_XPU float2 next_2d() {
        LUMINOUS_VAR_PTR_DISPATCH(next_2d)
    }

    static TestSampler create() {
        return TestSampler(get_arena().create<PCGSampler>(5));
    }
};

void test_upload() {
    auto device = luminous::create_cpu_device();
    auto &arena = get_arena();

    Managed<TestSampler> sp{device.get()};
    sp.reserve(1);
    auto sampler = TestSampler::create();

    sp.push_back(sampler);

    size_t size = luminous::lstd::Sizer<TestSampler>::max_size;

    cout << size << endl;

    arena.create_memory_block_and_focus(size);

    auto ts = TestSampler::create();

    cout << ts.next_2d().to_string() << endl;

    size_t s = 0;
    arena.for_each_block([&](MemoryArena::ConstBlockIterator iter){

        s += iter->usage();
        Buffer<std::byte>  buffer = device->create_buffer<std::byte>(s);

        buffer.upload(iter->address());

        PtrMapper::instance()->add_pair(iter->interval_used(),buffer.ptr_interval());

    });


    for_each_all_registered_member<TestSampler>([&](auto offset, auto name, auto ptr) {
        cout << name << "  " << offset <<endl;
        TestSampler &tss = sp.at(0);
        ptr_t p = get_ptr_value(&tss, offset);
        set_ptr_value(&tss, offset, PtrMapper::instance()->get_device_ptr(p));
//        set_ptr_value(&tss, offset, ptr_t(0));

    });

    cout << sp->next_2d().to_string() << endl;


    auto synchronizer = Synchronizer<TestSampler>(device.get());
    auto config = SamplerConfig();
    synchronizer.init(1);
    config.set_full_type("PCGSampler");
    synchronizer.add_element(config);

    cout << sp->next_2d().to_string() << endl;

}

class A : BASE_CLASS() {
public:
    REFL_CLASS(A)

    DEFINE_AND_REGISTER_MEMBER(A *, pa)

    virtual void func() {

    }
};

class B : BASE_CLASS(A) {
public:
    REFL_CLASS(B)

    DEFINE_AND_REGISTER_MEMBER(A*, p)
};


void test_ptr_access() {
    B *p = new B;
    A *p2 = new A;
    p->p = p2;

    set_ptr_value(p, 16, ptr_t(p2));

//    p->pa = p2;

    for_each_all_registered_member<B>([&](auto offset, auto name, auto ptr) {
        cout << offset << "   " << name << "  " << ::get_ptr_value(p, offset) << endl;
    });

    cout << "pa addr is " << uint64_t(p->pa) << endl;
    cout << "p addr is " << uint64_t(p->p) << endl;

}

void test_allocate_upload() {

}

int main() {
    test_upload();

    return 0;
}