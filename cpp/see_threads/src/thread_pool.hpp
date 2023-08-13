#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <set>
#include <thread>

#include "semaphore.hpp"
#include "task.hpp"
#include "worker.hpp"

#define RUNNING 1
#define STOPPING 2
#define STOPPED 3

namespace letMeSee {
class ThreadPool {
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
  int GetStatus();
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
  std::mutex commitLock;  // 总体🔒
  std::mutex listLock;    // 任务队列锁
  std::condition_variable closeCond;
  std::condition_variable listCond;

  int addTask(Task *task, bool isJumpQueue);
  bool createWorker(bool isCore, Task *task);
  Task *getTaskFromList();
  bool addTaskToListFront(Task *task);
  bool addTaskToListEnd(Task *task);
  void removeWorker(Worker *target);
};

}