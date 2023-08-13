#include "worker.hpp"

#include <iostream>

#include "thread_pool.hpp"

namespace letMeSee {
Worker::Worker() {}

Worker::Worker(bool isCore, Task *task, ThreadPool *tp)
    : isCore(isCore), task(task), tp(tp) {}

Worker::~Worker() {}

void Worker::Start() { t = std::thread(Worker::threadFunc, this); }

void Worker::Stop() { isStoped = true; }

void Worker::threadFunc(void *args) {
  Worker *self = (Worker *)args;
  ThreadPool *tp = self->tp;
  Task *task = self->task;
  self->task = nullptr;
  // 如果线程池不是停止的，并且任务不是空就卡住执行
  while (!(self->isStoped) &&
         (task != nullptr || (task = tp->getTaskFromList()))) {
    try {
      task->run();
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }
    delete task;
    task = nullptr;
    if (!self->isCore) break;
  }
  if (self->isCore) return;
  if (self->t.joinable()) {
    self->t.detach();
  }
  tp->removeWorker(self);
}

}  // namespace letMeSee