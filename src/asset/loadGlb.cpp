#include "loadGlb.hpp"
#include "readFileBytes.hpp"
#include "modelParseError.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

enum ChunkType
{
    JSON = 0x4E4F534A,
    BIN = 0x004E4942,
};

enum ComponentType
{
    BYTE = 5120,
    UNSIGNED_BYTE = 5121,
    SHORT = 5122,
    UNSIGNED_SHORT = 5123,
    UNSIGNED_INT = 5125,
    FLOAT = 5126,
};

uint32_t toUint32LE(uint32_t n)
{
    uint8_t *bytes = (uint8_t *)&n;
    return (uint32_t(bytes[0])) |
           (uint32_t(bytes[1]) << 8) |
           (uint32_t(bytes[2]) << 16) |
           (uint32_t(bytes[3]) << 24);
}
uint32_t readU32LE(const std::byte *p)
{
    uint32_t value;
    std::memcpy(&value, p, sizeof(value));
    return toUint32LE(value);
}

ParsedData loadGlb(const std::string &path)
{
    std::vector<std::byte> bytes = readFileBytes(path);
    std::byte *data = bytes.data();
    uint32_t magic = readU32LE(data);
    if (magic != 0x46546C67)
    {
        throw ModelParseError("GLB file corrupted");
    }
    uint32_t version = readU32LE(data + 4);
    std::cout << "Version: " << version << std::endl;
    uint32_t length = readU32LE(data + 8);
    uint32_t chunkLength = readU32LE(data + 12);
    std::cout << "Chunk length: " << chunkLength << std::endl;
    char *chunkStart = (char *)(data + 20);
    nlohmann::json j = nlohmann::json::parse(chunkStart, chunkStart + chunkLength);
    // std::cout << j.dump(2) << std::endl;

    nlohmann::json mesh = j["meshes"][0];
    nlohmann::json primitives = mesh["primitives"];
    nlohmann::json primitive = primitives[0];
    uint32_t posIndex = primitive["attributes"]["POSITION"];
    std::cout << "posIndex: " << posIndex << std::endl;
    uint32_t normIndex = primitive["attributes"]["NORMAL"];
    std::cout << "normIndex: " << normIndex << std::endl;
    uint32_t indicesIndex = primitive["indices"];
    std::cout << "indicesIndex: " << indicesIndex << std::endl;

    nlohmann::json accessors = j["accessors"];
    nlohmann::json posAccessor = accessors[posIndex];
    std::string posAccessorType = posAccessor["type"];
    uint32_t posAccessorComponentType = posAccessor["componentType"];
    if (posAccessorType != "VEC3" || posAccessorComponentType != ComponentType::FLOAT)
    {
        throw ModelParseError("Position accessor must be a 3D vector of floats");
    }
    uint32_t posBufferViewIndex = posAccessor["bufferView"];

    nlohmann::json normAccessor = accessors[normIndex];
    std::string normAccessorType = normAccessor["type"];
    uint32_t normAccessorComponentType = normAccessor["componentType"];
    if (normAccessorType != "VEC3" || normAccessorComponentType != ComponentType::FLOAT)
    {
        throw ModelParseError("Normal accessor must be a 3D vector of floats");
    }
    uint32_t normBufferViewIndex = normAccessor["bufferView"];

    nlohmann::json indicesAccessor = accessors[indicesIndex];
    uint32_t indicesBufferViewIndex = indicesAccessor["bufferView"];
    std::string indicesAccessorType = indicesAccessor["type"];
    uint32_t indicesAccessorComponentType = indicesAccessor["componentType"];
    if (indicesAccessorType != "SCALAR" || indicesAccessorComponentType != ComponentType::UNSIGNED_INT)
    {
        throw ModelParseError("Indices accessor must be a scalar unsigned integer");
    }

    nlohmann::json bufferViews = j["bufferViews"];
    nlohmann::json posBufferView = bufferViews[posBufferViewIndex];
    std::cout << "posBufferView: " << posBufferView << std::endl;
    nlohmann::json normBufferView = bufferViews[normBufferViewIndex];
    std::cout << "normBufferView: " << normBufferView << std::endl;
    nlohmann::json indicesBufferView = bufferViews[indicesBufferViewIndex];
    std::cout << "indicesBufferView: " << indicesBufferView << std::endl;

    // std::cout << accessors.dump(2) << std::endl;

    return ParsedData();
}
