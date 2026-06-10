#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../math/linalg/vec3.hpp"

struct Vertex {
    Vec3 position;
    Vec3 normal;
};

struct ParsedData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

ParsedData loadGlb(const std::string &path);
