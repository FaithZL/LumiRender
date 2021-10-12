//
// Created by Zero on 2021/4/10.
//


#pragma once

#include "base_libs/header.h"

namespace luminous {

    ND_XPU_INLINE size_t fix_count(size_t offset, size_t count, size_t size) {
        count = count < (size - offset) ? count : (size - offset);
        return count;
    }

    template<typename T = std::byte>
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

        LM_XPU BufferView()
                : _ptr(nullptr), _num(0) {}

        LM_XPU BufferView(T *ptr, size_t num)
                : _ptr(ptr), _num(num) {}

        LM_ND_XPU T *ptr() { return _ptr; }

        LM_ND_XPU const T *ptr() const { return _ptr; }

        template<typename target_type>
        LM_ND_XPU BufferView<target_type> cast() const {
            size_t num = _num * float(sizeof(value_type)) / sizeof(target_type);
            return BufferView<target_type>(reinterpret_cast<target_type *>(_ptr), num);
        }

        LM_ND_XPU iterator begin() { return _ptr; }

        LM_ND_XPU iterator end() { return _ptr + _num; }

        LM_ND_XPU const_iterator cbegin() const { return _ptr; }

        LM_ND_XPU const_iterator cend() const { return _ptr + _num; }

        template<typename Index>
        LM_ND_XPU T &operator[](Index i) {
            EXE_DEBUG(i >= size(), printf("ptr = %p, size:%d,index:%d\n", _ptr, int(size()), int(i)));
#ifndef NDEBUG
            if (i >= size()) {
                volatile int a = 0;
            }
#endif
            DCHECK_LT(i, size());
            return _ptr[i];
        }

        template<typename Index>
        LM_ND_XPU const T &operator[](Index i) const {
            EXE_DEBUG(i >= size(), printf("ptr = %p, size:%d,index:%d\n", _ptr, int(size()), int(i)));
#ifndef NDEBUG
            if (i >= size()) {
                volatile int a = 0;
            }
#endif
            DCHECK_LT(i, size());
            return _ptr[i];
        }

        LM_ND_XPU bool empty() const { return _num == 0; }

        LM_ND_XPU T front() const { return _ptr[0]; }

        LM_ND_XPU T back() const { return _ptr[_num - 1]; }

        LM_ND_XPU BufferView sub_view(size_t offset = 0, size_t count = -1) const {
            count = fix_count(offset, count, size());
            return BufferView(_ptr + offset, count);
        }

        LM_ND_XPU size_t size() const { return _num; }
    };
}