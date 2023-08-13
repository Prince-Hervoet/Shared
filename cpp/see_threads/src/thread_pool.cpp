#include "thread_pool.hpp"

#include <iostream>
#include <memory>

namespace letMeSee {

ThreadPool::ThreadPool() {}

ThreadPool::ThreadPool(int coreThreadCapacity, int threadCapacity,
                       int taskCapacity)
    : coreThreadCapacity(coreThreadCapacity),
      threadCapacity(threadCapacity),
      taskCapacity(taskCapacity) {}

ThreadPool::~ThreadPool() { Close(); }

int ThreadPool::Commit(Task *task) { return addTask(task, false); }

int ThreadPool::JumpQueue(Task *task) { return addTask(task, true); }

int ThreadPool::addTask(Task *task, bool isJumpQueue) {
  if (status != RUNNING) return 0;
  // 判断核心线程数
  if (coreThreadSize < coreThreadCapacity) {
    if (createWorker(true, task)) return 1;
    if (status != RUNNING) return 0;
  }
  // 加入队列
  if (isJumpQueue) {
    if (addTaskToListEnd(task)) return 1;
  } else {
    if (addTaskToListFront(task)) return 1;
  }
  if (createWorker(false, task)) return 1;
  if (status != RUNNING) return 0;
  return -1;
}

int ThreadPool::TaskSize() { return tasks.size(); }

int ThreadPool::ThreadSize() { return workers.size(); }

int ThreadPool::TaskCapacity() { return taskCapacity; }

void ThreadPool::Close() {
  {
    std::unique_lock<std::mutex> lock(commitLock);
    if (status != RUNNING) return;
    status = STOPPING;
    // 打上停止标记
    for (auto it = workers.begin(); it != workers.end(); it++) {
      Worker *w = *it;
      w->Stop();
    }
    // 唤醒所有在任务队列等待的线程
    listCond.notify_all();
  }
  // 主线程等待回收子线程
  for (auto it = workers.begin(); it != workers.end(); it++) {
    Worker *w = *it;
    if (w->t.joinable()) {
      w->t.join();
    }
  }

  for (auto it = workers.begin(); it != workers.end(); it++) {
    Worker *w = *it;
    delete w;
  }
  while (!tasks.empty()) {
    delete tasks.front();
    tasks.pop_front();
  }

  this->threadSize = 0;
  this->coreThreadSize = 0;
  status = STOPPED;
}

bool ThreadPool::createWorker(bool isCore, Task *task) {
  std::unique_lock<std::mutex> lock(commitLock);
  if ((coreThreadSize == coreThreadCapacity && isCore) || status != RUNNING)
    return false;
  if (threadSize == threadCapacity) return false;
  Worker *worker = new Worker(isCore, task, this);
  workers.insert(worker);
  worker->Start();
  if (isCore) coreThreadSize += 1;
  threadSize += 1;
  return true;
}

bool ThreadPool::addTaskToListFront(Task *task) {
  std::unique_lock<std::mutex> lock(listLock);
  if (tasks.size() == taskCapacity || status != RUNNING) return false;
  tasks.push_front(task);
  listCond.notify_one();
  return true;
}

bool ThreadPool::addTaskToListEnd(Task *task) {
  std::unique_lock<std::mutex> lock(listLock);
  if (tasks.size() == taskCapacity || status != RUNNING) return false;
  tasks.push_back(task);
  listCond.notify_one();
  return true;
}

Task *ThreadPool::getTaskFromList() {
  std::unique_lock<std::mutex> lock(listLock);
  while (tasks.size() == 0) {
    if (status != RUNNING) return nullptr;
    listCond.wait(lock);
  }
  Task *task = tasks.back();
  tasks.pop_back();
  return task;
}

void ThreadPool::removeWorker(Worker *target) {
  if (target == nullptr) return;
  std::unique_lock<std::mutex> lock(commitLock);
  if (status != RUNNING) return;
  workers.erase(target);
  delete target;
}

int ThreadPool::GetStatus() { return status; }

}  // namespace letMeSee