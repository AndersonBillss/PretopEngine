#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include "AssetLoader.hpp"
#include "../Math/Linalg/Vec3.hpp"

namespace Pretop::Asset
{
    struct Vertex
    {
        Math::Vec3 Position;
        Math::Vec3 Normal;
    };

    using IndexList = std::variant<std::vector<uint16_t>, std::vector<uint32_t>>;

    struct ParsedData
    {
        std::vector<Vertex> Vertices;
        IndexList Indices;
    };

    ParsedData LoadGlb(AssetLoader *assetLoader, const std::string &path);
} // namespace Pretop::Asset
