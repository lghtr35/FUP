#ifndef FUP_THREAD_POOL_HPP
#define FUP_THREAD_POOL_HPP
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
    public:
        Threadpool(size_t);
        template<class F, class... Args>
        void Enqueue(F&& f, Args&&... args);
        ~Threadpool();
    }; 
}

#endif