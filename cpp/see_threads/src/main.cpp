#include <iostream>

#include "thread_pool.hpp"

class TestTask : public letMeSee::Task {
 public:
  TestTask() {}

  ~TestTask() {}

  void run() { std::cout << "asdf" << std::endl; }
};

int main() {
  letMeSee::ThreadPool *tp = new letMeSee::ThreadPool(5, 20, 100);

  for (int i = 0; i < 3; i++) {
    letMeSee::Task *task = new TestTask();
    tp->Commit(task);
  }

  tp->Close();
  delete tp;

  for (;;) {
  }
  return 0;
}