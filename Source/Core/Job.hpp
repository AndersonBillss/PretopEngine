#pragma once

using JobFn = void (*)(void *);

namespace Pretop::Core
{
    struct Job
    {
        JobFn fn = nullptr;
        void *userData = nullptr;
    };
}
