//
// Created by Zero on 2021/1/31.
//

#include "graphics/lstd/lstd.h"
#include "graphics/common.h"
#include "iostream"

using namespace luminous;
using namespace std;

class Sub1 {
public:
    Sub1() {
        std::cout << "construct sub1\n";
    }

    int fun1() {
        return 0;
    }

    int fun2(int a) {
        return a;
    }

    ~Sub1() {
        std::cout << "destruct sub1 " << this << endl;
    }
};

class Sub2 {
public:
    Sub2() {
        std::cout << "construct sub2\n";
    }
    int fun1() {
        return 1;
    }

    int fun2(int a) {
        return 2 * a;
    }

    ~Sub2() {
        std::cout << "destruct sub2 " << this << endl;
    }
};

using ::lstd::Variant;

class Base : public Variant<Sub1, Sub2> {
public:
    using Variant::Variant;

    int fun1() {
        return dispatch([](auto &&arg) { return arg.fun1(); });
    }

    int fun2(int a) {
        LUMINOUS_VAR_DISPATCH(fun2, a);
    }
};

class BaseP : public Variant<Sub1 *, Sub2 *> {
public:
    using Variant::Variant;

    int fun1() {
        return dispatch([](auto &&arg) { return arg->fun1(); });
    }

    int fun2(int a) {
        LUMINOUS_VAR_PTR_DISPATCH(fun2, a);
    }
};

using namespace std;

//void testVariant() {
////    Sub1 s1 = Sub1();
////    Sub2 s2 = Sub2();
////
////    cout << s1.fun1() << endl;
////    cout << s2.fun1() << endl;
//
////    Base b = Sub1();
////
////    Base b2 = Sub2();
////
////    cout << sizeof(b) << endl;
////    cout << sizeof(s2) << endl;
//
////
////    cout << b.fun1() << endl;
////    cout << b.fun2(9) << endl;
//
//    BaseP bp = new Sub1;
//
//    BaseP bp2 = new Sub2;
//
//    auto aa = new Sub1();
//
//    cout << is_pointer<decltype(aa)>::value << endl;
//    cout << is_pointer<decltype(bp)>::value << endl;
//
////    cout << bp.fun1() << endl;
////    cout << bp.fun2(9) << endl;
////
////    cout << bp2.fun1() << endl;
////    cout << bp2.fun2(9) << endl;
//
//
//}

void test_math() {
//    Box3f box(make_float3(3.1));
//    cout << box.to_string() << endl;

//    auto a = int3x3(3.234f) ;
//    cout << a.to_string();

    auto m1 = make_double4x4(-2);

//    auto v1 = make_double4(1);
//    auto v2 = v1;
//    cout << m1.to_string();
//
//    auto v3 = v2 * v1;
//    cout << inverse(m1).to_string() << endl;
//    cout << transpose(m1).to_string() << endl;
    auto m2 = inverse(m1);

//    auto m3 =  m2 * m1;
    cout << m2.to_string() << endl;
}

void test_transform() {
    auto t = make_float3(1, 2, 3);
    auto tsf = Transform::translation(t);

    auto r = Transform::rotation(make_float3(3, 1, 2), 30);
    auto s = Transform::scale(make_float3(3, 4, 9));

    tsf = tsf * r * s;

    auto inv = tsf.inverse();

    auto p = make_float3(5, 6, 7);

    cout << tsf.mat4x4().to_string() << endl;
    auto np = tsf.apply_normal(p);
    auto nnp = inv.apply_normal(np);
    cout << p.to_string() << endl;
    cout << np.to_string() << endl;
    cout << nnp.to_string() << endl;

    np = tsf.apply_vector(p);
    nnp = inv.apply_vector(np);
    cout << p.to_string() << endl;
    cout << np.to_string() << endl;
    cout << nnp.to_string() << endl;

    np = tsf.apply_point(p);
    nnp = inv.apply_point(np);
    cout << p.to_string() << endl;
    cout << np.to_string() << endl;
    cout << nnp.to_string() << endl;

    float3 tt;
    Quaternion rr;
    float3 ss;
    decompose(tsf.mat4x4(), &tt, &rr, &ss);
    cout << tsf.to_string_detail();
//    cout << tt.to_string() << endl << rr.to_string() << endl << ss.to_string() << endl;
}


void test_color() {
    auto s = Spectrum::linear_to_srgb(make_float3(0.1, 0.2, 0.3));
    auto l = Spectrum::srgb_to_linear(s);
    cout << s.to_string() << endl;
    cout << l.to_string() << endl;
    cout << Spectrum::linear_to_srgb(Spectrum(make_float3(0.3))).to_string() << endl;
    cout << Spectrum::srgb_to_linear(Spectrum(make_float3(0.583831))).to_string() << endl;

    auto c = Spectrum(make_float3(0));
    cout << c.luminance() << endl;
    cout << "is black " <<c.is_black() << endl;
    cout << sizeof(Spectrum) << endl;
    cout << sizeof(float3) << endl;
}

#include "new"

void piecewise_construct_test() {
    float arr[] = {0,2};
    using ::lstd::span;
    auto sp = span<float>(arr,2);

    auto dis = PiecewiseConstant1D(sp);

    float pdf;
    int ofs;
    cout << dis.Sample(0.5, &pdf, &ofs) << endl;
    cout << pdf << endl << ofs << endl;
    cout << dis.Invert(0.999).value();

}

void piecewise2d_test() {
    float arr[] = {0,1,1,2};
    using ::lstd::span;
    auto sp = span<float>(arr, 4);
    auto dis = PiecewiseConstant2D(sp, 2, 2);

    auto u = make_float2(0.5, 0.5);
    float pdf;
    cout << dis.Sample(u, &pdf).to_string() << endl;
    cout << pdf;
}

int main() {

//    testVariant();

//    test_math();
//    test_transform();

//    test_color();

//    piecewise_construct_test();

//    piecewise2d_test();
    return 0;
}