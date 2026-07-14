#pragma once

using MainThreadFn = void (*)(void *);

namespace Pretop::Core
{
    struct Completion
    {
        MainThreadFn done = nullptr;
    };
}
