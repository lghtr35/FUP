#include "thread_pool.hpp"

namespace fup {
    thread_pool::thread_pool(size_t threads) :stop(false) {
        for (size_t i = 0;i < threads;++i)
            workers.emplace_back(
                [this]
                {
                    for (;;)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->mutex);
                            this->condition.wait(lock,
                                [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
            );
    }

    // add new work item to the pool
    template<class F, class... Args>
    void thread_pool::enqueue(F&& f, Args&&... args)
    {
        auto task = std::make_shared< std::packaged_task<void()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped thread_pool");

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
    }

    // the destructor joins all threads
    inline thread_pool::~thread_pool()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }
}