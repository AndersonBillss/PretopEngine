#pragma once

using JobFn = void (*)(void *);

namespace Pretop::Core
{
    struct Job
    {
        JobFn Fn = nullptr;
        void *UserData = nullptr;
    };
}
