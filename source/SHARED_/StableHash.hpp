#pragma once
#include <cstdint>
#include <string_view>

namespace Pretop::Utils
{
    std::uint64_t StableHash(const std::string_view str);
}
