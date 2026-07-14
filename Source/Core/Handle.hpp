#pragma once
#include <cstdint>

namespace Pretop::Core
{
    struct Handle {
        uint32_t index;
        uint32_t generation;
    };
}
