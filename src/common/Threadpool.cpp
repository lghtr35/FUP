#include "Threadpool.hpp"

namespace fup {
    Threadpool::Threadpool(uint16_t threads) :stop(false) {
        if (threads == 0) {
            throw std::invalid_argument("Number of threads must be greater than 0");
        }
        for (uint16_t i = 0; i < threads; ++i)
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
                            ++working;
                        }

                        task();
                        
                        {
                            std::unique_lock<std::mutex> lock(this->mutex);
                            --working;
                        }
                    }
                }
            );
    }

    // wait for all tasks to finish
    void Threadpool::WaitAll()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }

    // get the number of working threads
    size_t Threadpool::GetWorkingThreadsCount()
    {
        std::unique_lock<std::mutex> lock(mutex);
        return working;
    }

    // the destructor joins all threads
    Threadpool::~Threadpool()
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