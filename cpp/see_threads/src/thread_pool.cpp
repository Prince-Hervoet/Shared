#include <memory>
#include <iostream>
#include "thread_pool.hpp"

namespace letMeSee
{

    ThreadPool::ThreadPool() {}

    ThreadPool::ThreadPool(int coreThreadCapacity, int threadCapacity, int taskCapacity)
        : coreThreadCapacity(coreThreadCapacity), threadCapacity(threadCapacity), taskCapacity(taskCapacity)
    {
    }

    int ThreadPool::Commit(Task *task)
    {
        if (status != RUNNING)
            return 0;
        // 判断核心线程数
        if (coreThreadSize < coreThreadCapacity)
        {
            if (createWorker(true, task))
                return 1;
            if (status != RUNNING)
                return 0;
        }
        // 加入队列
        if (addTaskToList(task))
            return 1;
        if (createWorker(false, task))
            return 1;
        if (status != RUNNING)
            return 0;
        return -1;
    }

    int ThreadPool::TaskSize()
    {
        return tasks.size();
    }

    int ThreadPool::ThreadSize()
    {
        return workers.size();
    }

    int ThreadPool::TaskCapacity()
    {
        return taskCapacity;
    }

    void ThreadPool::Close()
    {
        std::unique_lock<std::mutex> lock(commitLock);
        if (status != RUNNING)
            return;
        status = STOPPING;
        for (auto it = workers.begin(); it != workers.end(); it++)
        {
            Worker *w = *it;
            w->Stop();
        }

        listCond.notify_all();

        while (workers.size() > 0)
        {
            closeCond.wait(lock);
        }

        status = STOPPED;

        while (!tasks.empty())
        {
            delete tasks.front();
            tasks.pop_front();
        }
    }

    bool ThreadPool::createWorker(bool isCore, Task *task)
    {
        std::unique_lock<std::mutex> lock(commitLock);
        if ((coreThreadSize == coreThreadCapacity && isCore) || status != RUNNING)
            return false;
        if (threadSize == threadCapacity)
            return false;
        Worker *worker = new Worker(isCore, task, this);
        workers.insert(worker);
        worker->Start();
        if (isCore)
            coreThreadSize += 1;

        threadSize += 1;
        return true;
    }

    bool ThreadPool::addTaskToList(Task *task)
    {
        std::unique_lock<std::mutex> lock(listLock);
        if (tasks.size() == taskCapacity || status != RUNNING)
            return false;
        tasks.push_front(task);
        listCond.notify_one();
        return true;
    }

    Task *ThreadPool::getTaskFromList()
    {
        std::unique_lock<std::mutex> lock(listLock);
        while (tasks.size() == 0)
        {
            if (status != RUNNING)
                return nullptr;
            listCond.wait(lock);
        }
        Task *task = tasks.back();
        tasks.pop_back();
        return task;
    }

    void ThreadPool::removeWorker(Worker *target)
    {
        if (target == nullptr)
            return;

        std::unique_lock<std::mutex> lock(commitLock);
        workers.erase(target);
        delete target;
        closeCond.notify_one();
    }
}