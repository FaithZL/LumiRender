//
// Created by Zero on 2021/3/9.
//


#pragma once

#include "core/concepts.h"
#include "base_libs/math/common.h"
#include "kernel.h"

namespace luminous {
    class Module {
    public:
        class Impl {
        public:
            LM_NODISCARD virtual SP<KernelOld> get_kernel(const std::string &name) = 0;

            LM_NODISCARD virtual uint64_t get_kernel_handle(const std::string &name) = 0;
        };

        SP<KernelOld> get_kernel(const std::string &name) {
            return _impl->get_kernel(name);
        }

        explicit Module(std::unique_ptr<Impl> impl) : _impl(std::move(impl)) {}
    private:
        std::unique_ptr<Impl> _impl;
    };
}