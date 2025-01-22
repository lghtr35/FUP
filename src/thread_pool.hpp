#ifndef FUP_THREAD_POOL_HPP
#define FUP_THREAD_POOL_HPP
#include "common.hpp"
#include <thread>
#include <condition_variable>

namespace fup {
    class thread_pool
    {
    private:
        size_t max_thread_count;
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        
        std::mutex mutex;
        std::condition_variable condition;
        bool stop;
    public:
        thread_pool(size_t);
        template<class F, class... Args>
        void enqueue(F&& f, Args&&... args);
        ~thread_pool();
    }; 
}

#endif