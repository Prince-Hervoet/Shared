#include "thread_pool.hpp"
#include <iostream>

class TestTask : public letMeSee::Task
{
public:
    TestTask() {}

    ~TestTask() {}

    void run()
    {
        std::cout << "asdf" << std::endl;
    }
};

int main()
{
    letMeSee::ThreadPool *tp = new letMeSee::ThreadPool(5, 20, 100);

    for (int i = 0; i < 10; i++)
    {
        letMeSee::Task *task = new TestTask();
        tp->Commit(task);
    }

    tp->Close();
    // letMeSee::Task *task = new TestTask();
    // tp->Commit(task);
    for (;;)
    {
        std::cout << tp->ThreadSize() << std::endl;
    }
    return 0;
}