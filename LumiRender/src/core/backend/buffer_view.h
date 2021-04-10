//
// Created by Zero on 2021/4/10.
//


#pragma once

#include "graphics/header.h"

namespace luminous {
    template<typename T>
    struct BufferView {
    private:
        // point to device memory or host memory
        T *_ptr;
        // count of elements
        size_t _num;
    public:
        using value_type = T;
        using iterator = T *;
        using const_iterator = const T *;

        XPU BufferView() : _ptr(nullptr), _num(0) {}

        XPU BufferView(T *ptr, size_t num)
                : _ptr(ptr), _num(num) {}

        NDSC_XPU iterator begin() { return _ptr; }

        NDSC_XPU iterator end() { return _ptr + _num; }

        NDSC_XPU const_iterator c_begin() const { return _ptr; }

        NDSC_XPU const_iterator c_end() const { return _ptr + _num; }

        NDSC_XPU T &operator[](size_t i) {
            DCHECK_LT(i, size());
            return _ptr[i];
        }

        NDSC_XPU bool empty() const { return _num == 0; }

        NDSC_XPU T front() const { return _ptr[0]; }

        NDSC_XPU T back() const { return _ptr[_num - 1]; }

        NDSC_XPU BufferView sub_view(size_t pos, size_t count) {
            size_t np = count < (size() - pos) ? count : (size() - pos);
            return BufferView(_ptr + pos, np);
        }

        NDSC_XPU size_t size() const {
            return _num;
        }
    };
}