#include "semaphore.hpp"

namespace letMeSee
{
    Semaphore::Semaphore()
    {
        sem_init(&sem, 0, 1);
    }

    Semaphore::Semaphore(int value)
    {
        sem_init(&sem, 0, value);
    }

    Semaphore::~Semaphore()
    {
        sem_destroy(&sem);
    }

    void Semaphore::post()
    {
        sem_post(&sem);
    }

    void Semaphore::wait()
    {
        sem_wait(&sem);
    }

}