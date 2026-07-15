#pragma once
#include "Handle.hpp"

using MainThreadFn = void (*)(Pretop::Core::Handle, void *);

namespace Pretop::Core
{
    struct Completion
    {
        MainThreadFn done = nullptr;
    };
}
