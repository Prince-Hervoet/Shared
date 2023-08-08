#pragma once

#include <semaphore.h>

namespace letMeSee
{
    class Semaphore
    {
    public:
        Semaphore();

        Semaphore(int value);

        ~Semaphore();

        void post();

        void wait();

    private:
        sem_t sem;
    };
}