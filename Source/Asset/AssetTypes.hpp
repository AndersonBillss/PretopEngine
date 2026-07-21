#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace Pretop::Asset
{
    using AssetId = std::string;

    enum class AssetKind
    {
        Text,
        Binary,
        Model,
    };

    using AssetBytes = std::vector<std::byte>;
    using AssetText = std::string;

    template <typename T>
    struct AssetResult
    {
        T Data;
        std::string Error;

        explicit operator bool() const noexcept
        {
            return Error.empty();
        }
    };
} // namespace Pretop::Asset
