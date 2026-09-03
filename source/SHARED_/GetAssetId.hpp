#pragma once
#include "NormalizePath.hpp"
#include "StableHash.hpp"

namespace Pretop::Utils
{
    inline uint64_t GetAssetId(const std::string_view path)
    {
        return StableHash(NormalizePath(path));
    }
}
