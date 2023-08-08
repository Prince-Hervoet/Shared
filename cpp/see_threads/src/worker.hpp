#pragma once

#include <thread>
#include "task.hpp"

namespace letMeSee
{
    class ThreadPool;
    class Worker
    {
    public:
        Worker();
        Worker(bool isCore, Task *task, ThreadPool *tp);
        ~Worker();

        void Start();
        void Stop();

    private:
        ThreadPool *tp;
        Task *task;
        volatile bool isStoped;
        bool isCore;
        static void threadFunc(void *args);
    };
}