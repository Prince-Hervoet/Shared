#pragma once

#include <set>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "task.hpp"
#include "worker.hpp"
#include "semaphore.hpp"

#define RUNNING 1
#define STOPPING 2
#define STOPPED 3

namespace letMeSee
{
    class ThreadPool
    {
        friend class Worker;

    public:
        ThreadPool();
        ThreadPool(int coreThreadCapacity, int threadCapacity, int taskCapacity);
        ~ThreadPool();

        int Commit(Task *task);
        int JumpQueue(Task *task);
        int TaskSize();
        int ThreadSize();
        int TaskCapacity();
        void Close();

    private:
        int coreThreadSize = 0;
        int threadSize = 0;
        int coreThreadCapacity = 0;
        int threadCapacity = 0;
        int taskCapacity = 0;
        volatile int status = RUNNING;
        std::set<Worker *> workers;
        std::list<Task *> tasks;
        std::mutex commitLock;
        std::mutex listLock;
        std::mutex workersLock;
        std::condition_variable listCond;
        std::condition_variable closeCond;

        bool createWorker(bool isCore, Task *task);
        Task *getTaskFromList();
        bool addTaskToList(Task *task);
        void removeWorker(Worker *target);
    };

}