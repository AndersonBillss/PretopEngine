#pragma once
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include "AssetLoader.hpp"
#include "../Math/Linalg/Vec3.hpp"

struct Vertex {
    Vec3 Position;
    Vec3 Normal;
};

using IndexList = std::variant<std::vector<uint16_t>, std::vector<uint32_t>>;

struct ParsedData {
    std::vector<Vertex> Vertices;
    IndexList Indices;
};

ParsedData LoadGlb(AssetLoader *assetLoader, const std::string &path);
