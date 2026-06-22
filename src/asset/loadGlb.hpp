#pragma once
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include "assetLoader.hpp"
#include "../math/linalg/vec3.hpp"

struct Vertex {
    Vec3 position;
    Vec3 normal;
};

using IndexList = std::variant<std::vector<uint16_t>, std::vector<uint32_t>>;

struct ParsedData {
    std::vector<Vertex> vertices;
    IndexList indices;
};

ParsedData loadGlb(AssetLoader * assetLoader, const std::string &path);
