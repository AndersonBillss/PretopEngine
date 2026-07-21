#pragma once
#include <cstdint>
#include <vector>
#include <variant>
#include "AssetTypes.hpp"
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

    ParsedData LoadGlb(const AssetBytes &bytes);
} // namespace Pretop::Asset
