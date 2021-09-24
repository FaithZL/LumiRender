#pragma once

#include "core/refl/factory.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace luminous;


#include <iostream>

template<typename...Bases>
struct RegisterBase : Bases ... {
    using Ts = std::tuple<Bases...>;
};

template<typename T>
struct A : public RegisterBase<>{
    static void foo() {
        std::cout << "A" << std::endl;
    }
};

template<typename T>
struct B : RegisterBase<A<T>> {
    static void foo() {
        std::cout << "B" << std::endl;
    }
};

struct C : RegisterBase<> {

};

template<typename T>
struct D : RegisterBase<B<T>, C> {

};

template<typename T, typename F, int...Is>
void forEachDirectBaseAux(const F &f, std::integer_sequence<int, Is...>) {
    (f.template operator()<std::tuple_element_t<Is, typename T::Ts>>(), ...);
}

template<typename T, typename F>
void forEachDirectBase(const F &f) {
    forEachDirectBaseAux<T>(
            f, std::make_integer_sequence<int, std::tuple_size_v<typename T::Ts>>());
}

struct Visitor {
    template<typename T>
    void operator()() const {
        forEachDirectBase<T>(*this);
        std::cout << typeid(T).name() << std::endl;

    }
};

struct TT : RegisterBase<C> {

};

int main() {

    Visitor visitor;

    forEachDirectBase<D<int>>(visitor);
}
