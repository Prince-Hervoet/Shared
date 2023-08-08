#pragma once

namespace letMeSee
{
    class Task
    {
    public:
        Task();
        virtual ~Task();
        virtual void run() = 0;
    };

}