#include "NormalizePath.hpp"
#include "StableHash.hpp"

namespace Pretop::Utils
{
    uint64_t GetAssetId(const std::string_view path)
    {
        return StableHash(NormalizePath(path));
    }
}
