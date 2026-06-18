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

template <typename T>
struct AssetResult
{
    std::shared_ptr<const T> data;
    std::string error;

    bool ok() const noexcept
    {
        return data != nullptr && error.empty();
    }

    explicit operator bool() const noexcept
    {
        return ok();
    }
};