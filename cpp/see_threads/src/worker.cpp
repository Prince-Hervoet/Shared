#include <iostream>
#include "worker.hpp"
#include "thread_pool.hpp"

namespace letMeSee
{
    Worker::Worker()
    {
    }

    Worker::Worker(bool isCore, Task *task, ThreadPool *tp)
        : isCore(isCore), task(task), tp(tp)
    {
    }

    Worker::~Worker()
    {
        delete task;
    }

    void Worker::Start()
    {
        std::thread t(Worker::threadFunc, this);
        t.detach();
    }

    void Worker::Stop()
    {
        isStoped = true;
    }

    void Worker::threadFunc(void *args)
    {
        Worker *self = (Worker *)args;
        ThreadPool *tp = self->tp;
        Task *task = self->task;
        while (!(self->isStoped) && (task != nullptr || (task = tp->getTaskFromList())))
        {
            try
            {
                task->run();
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
            }
            delete task;
            task = nullptr;
            if (!self->isCore)
                break;
        }
        tp->removeWorker(self);
    }

}