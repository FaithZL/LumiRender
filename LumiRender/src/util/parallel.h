//
// Created by Zero on 2020/11/3.
//

#pragma once

#include <atomic>
#include <functional>
#include <condition_variable>
#include <deque>
#include <future>
#include <base_libs/geometry/box.h>
#include "base_libs/math/common.h"

namespace luminous {
    inline namespace utility {

        int num_work_threads();

        struct ParallelForContext {
            std::atomic_uint32_t work_index;
            size_t count = 0;
            uint32_t chunk_size = 0;

            ParallelForContext() : work_index(0) {}

            std::function<void(uint32_t, uint32_t)> func{};

            bool done() const { return work_index >= count; }

            ParallelForContext(const ParallelForContext &rhs)
            : work_index(rhs.work_index.load()), count(rhs.count), chunk_size(rhs.chunk_size), func(rhs.func) {}
        };

        struct ParallelForWorkPool {
            std::deque<ParallelForContext> works;
            std::vector<std::thread> threads;
            std::condition_variable has_work, one_thread_finished, main_waiting;
            std::mutex work_mutex;
            std::atomic_bool stopped;
            std::uint32_t work_id;
            std::uint32_t n_thread_finished;

            ParallelForWorkPool() : work_id(0), n_thread_finished(0) {
                stopped = false;
                auto n = num_work_threads();
                for (uint32_t tid = 0; tid < n; tid++) {
                    threads.emplace_back([=]() {
                        while (!stopped) {
                            std::unique_lock<std::mutex> lock(work_mutex);
                            while (works.empty() && !stopped) {
                                has_work.wait(lock);
                            }
                            if (stopped)
                                return;
                            auto &loop = works.front();
                            auto id = work_id;
                            lock.unlock();
                            // lock held
                            while (!loop.done()) {
                                auto begin = loop.work_index.fetch_add(loop.chunk_size);
                                for (auto i = begin; i < begin + loop.chunk_size && i < loop.count; i++) {
                                    (loop.func)(i, tid);
                                }
                            }
                            lock.lock();
                            n_thread_finished++;
                            one_thread_finished.notify_all();

                            while (n_thread_finished != (uint32_t) threads.size() && work_id == id) {
                                one_thread_finished.wait(lock);
                            }

                            if (work_id == id) {
                                work_id++; // only one thread would reach here
                                works.pop_front();
                                if (works.empty()) {
                                    main_waiting.notify_one();
                                }
                                n_thread_finished = 0;
                            }
                            lock.unlock();
                        }
                    });
                }
            }

            void enqueue(const ParallelForContext &context) {
                std::lock_guard<std::mutex> lock(work_mutex);
                works.emplace_back(context);
                has_work.notify_all();
            }

            void wait() {
                std::unique_lock<std::mutex> lock(work_mutex);
                while (!works.empty()) {

                    main_waiting.wait(lock);
                }
            }

            ~ParallelForWorkPool() {
                stopped = true;
                has_work.notify_all();
                for (auto &thr : threads) {
                    thr.join();
                }
            }
        };

        void init_thread_pool();

        ParallelForWorkPool *work_pool();

        void set_thread_num(int num);

        class AtomicFloat {
        private:
            std::atomic<float> val;

        public:
            using Float = float;

            explicit AtomicFloat(Float v = 0) : val(v) {}

            AtomicFloat(const AtomicFloat &rhs) : val((float) rhs.val) {}

            void add(Float v) {
                auto current = val.load();
                while (!val.compare_exchange_weak(current, current + v)) {
                }
            }

            [[nodiscard]] float value() const { return val.load(); }

            explicit operator float() const { return value(); }

            void set(Float v) { val = v; }
        };

        void parallel_for(int count, const std::function<void(uint32_t, uint32_t)> &func, size_t chunkSize = 1);

        inline void parallel_for_2d(const uint2 &dim, const std::function<void(uint2, uint32_t)> &func,
                                    size_t chunkSize = 1) {
            parallel_for(
                    dim.x * dim.y,
                    [&](uint32_t idx, int tid) {
                        auto x = idx % dim.x;
                        auto y = idx / dim.x;
                        func(make_uint2(x, y), tid);
                    },
                    chunkSize);
        }

        void async(int count, const std::function<void(uint32_t, uint32_t)> &func, size_t chunkSize = 1);


        template<class F>
        void tiled_for_2d(const uint2 &dim, const uint2 &tile_size, F &&func, bool parallel = true) {
            uint2 n_tiles = (dim + tile_size - 1u) / tile_size;
            auto tile_func = [&](uint2 tile, uint thread_id) {
                uint2 p_min = tile * tile_size;
                uint2 p_max = p_min + tile_size;
                p_max = select(p_max > dim, dim, p_max);
                Box2u tile_bound{p_min, p_max};
                tile_bound.for_each([&](uint2 pixel) {
                    func(pixel, thread_id);
                });
            };
            if (parallel) {
                parallel_for_2d(n_tiles, tile_func);
            } else {
                for (int i = 0; i < n_tiles[0]; ++i) {
                    for (int j = 0; j < n_tiles[1]; ++j) {
                        tile_func(make_uint2(i, j), 0);
                    }
                }
            }
        }

        namespace thread {
            void finalize();
        }

        template<typename T>
        class Future {
            std::future<T> inner;

            template<typename R>
            friend
            class Future;

        public:
            Future(std::future<T> ft) : inner(std::move(ft)) {}

            template<typename F, typename R = std::invoke_result_t<F, decltype(std::declval<std::future<T>>().get())>>
            auto then(F &&f, std::launch policy = std::launch::deferred) -> Future<R> {
                return Future<R>(std::async(std::launch::deferred, [=, ft = std::move(inner)]() mutable {
                    if constexpr (std::is_same_v<T, void>) {
                        ft.get();
                        return f();
                    } else {
                        decltype(auto) result = ft.get();
                        return f(result);
                    }
                }));
            }
        };

        template<class _Fty, class... _ArgTypes>
        Future<std::invoke_result_t<std::decay_t<_Fty>, std::decay_t<_ArgTypes>...>>
        async_do(std::launch policy, _Fty &&_Fn_arg, _ArgTypes &&... _Args) {
            return std::async(policy, std::forward<_Fty>(_Fn_arg), std::forward<_ArgTypes>(_Args)...);
        }
    }
}