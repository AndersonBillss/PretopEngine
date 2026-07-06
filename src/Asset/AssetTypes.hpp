#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

using AssetId = std::string;

enum class AssetKind
{
    Text,
    Binary,
};

using AssetBytes = std::vector<std::byte>;
using AssetText = std::string;

template<typename T>
struct AssetResult
{
    T data;
    std::string error;

    explicit operator bool() const noexcept
    {
        return error.empty();
    }
};