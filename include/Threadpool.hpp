#pragma once

#include "Common.hpp"
#include <thread>
#include <condition_variable>

namespace fup {
    class Threadpool
    {
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex mutex;
        std::condition_variable condition;
        bool stop;
        size_t working;
    public:
        Threadpool(uint16_t);
        void WaitAll();
        size_t GetWorkingThreadsCount();
        ~Threadpool();


        template<class F>
        void Enqueue(F&& f)
        {
            using return_type = typename std::invoke_result<F>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(mutex);

                if (stop)
                    throw std::runtime_error("enqueue on stopped Threadpool");

                tasks.emplace([task]() { (*task)(); });
            }
            condition.notify_one();
        }
    };
}
