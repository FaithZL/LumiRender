//
// Created by Zero on 2021/1/31.
//

#include "core/lstd/lstd.h"
#include "graphics/common.h"

using namespace luminous;

class Sub1 {
public:
    int fun1() {
        return 0;
    }

    int fun2(int a) {
        return a;
    }
};

class Sub2 {
public:
    int fun1() {
        return 1;
    }

    int fun2(int a) {
        return  2* a;
    }
};

class Base : public lstd::Variant<Sub1, Sub2> {
public:
    using Variant::Variant;
    int fun1() {
        return dispatch([](auto &&arg) { return arg.fun1(); });
    }

    int fun2(int a) {
        LUMINOUS_VAR_DISPATCH(fun2, a);
    }
};

class BaseP : public lstd::Variant<Sub1 *, Sub2 * > {
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

void testVariant() {
    Sub1 s1 = Sub1();
    Sub2 s2 = Sub2();

    cout << s1.fun1() << endl;
    cout << s2.fun1() << endl;

    Base b = s2;

    Base b2 = s1;

    cout << b.fun1() << endl;
    cout << b.fun2(9) << endl;

    BaseP bp = &s1;

    BaseP bp2 = &s2;

    cout << bp.fun1() << endl;
    cout << bp.fun2(9) << endl;

    cout << bp2.fun1() << endl;
    cout << bp2.fun2(9) << endl;
}

void test_math() {
//    Box3f box(make_float3(3.1));
//    cout << box.to_string() << endl;

//    auto a = int3x3(3.234f) ;
//    cout << a.to_string();
}

int main() {

    testVariant();

    test_math();

    return 0;
}