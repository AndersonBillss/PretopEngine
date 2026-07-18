#pragma once
#include "Handle.hpp"

using MainThreadFn = void (*)(void *);

namespace Pretop::Core
{
    struct Completion
    {
        MainThreadFn Done = nullptr;
    };
}
