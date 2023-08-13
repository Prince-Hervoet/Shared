#pragma once

#include <thread>

#include "task.hpp"

namespace letMeSee {
class ThreadPool;
class Worker {
  friend class ThreadPool;

 public:
  Worker();
  Worker(bool isCore, Task *task, ThreadPool *tp);
  ~Worker();

  void Start();
  void Stop();

 private:
  std::thread t;
  ThreadPool *tp;
  Task *task;
  volatile bool isStoped;
  bool isCore;
  static void threadFunc(void *args);
};
}  // namespace letMeSee