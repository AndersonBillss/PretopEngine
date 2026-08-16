#pragma once
#include <cstdint>

namespace Pretop::Core
{
    struct Handle {
        uint32_t Index;
        uint32_t Generation;
    };
}
