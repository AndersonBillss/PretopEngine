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
float readFloatLE(const std::byte *p)
{
    uint32_t bits = readU32LE(p);

    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
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
    uint32_t jsonChunkLength = readU32LE(data + 12);
    uint32_t jsonChunkType = readU32LE(data + 16);
    if (jsonChunkType != ChunkType::JSON)
    {
        throw ModelParseError("First chunk is not JSON");
    }
    std::cout << "Chunk length: " << jsonChunkLength << std::endl;
    char *jsonChunkStart = (char *)(data + 20);
    char *jsonChunkEnd = jsonChunkStart + jsonChunkLength;
    nlohmann::json j = nlohmann::json::parse(jsonChunkStart, jsonChunkEnd);
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
        throw ModelParseError("Position accessor is not a 3D vector of floats");
    }
    uint32_t posBufferViewIndex = posAccessor["bufferView"];

    nlohmann::json normAccessor = accessors[normIndex];
    std::string normAccessorType = normAccessor["type"];
    uint32_t normAccessorComponentType = normAccessor["componentType"];
    if (normAccessorType != "VEC3" || normAccessorComponentType != ComponentType::FLOAT)
    {
        throw ModelParseError("Normal accessor is not a 3D vector of floats");
    }
    uint32_t normBufferViewIndex = normAccessor["bufferView"];

    nlohmann::json indicesAccessor = accessors[indicesIndex];
    uint32_t indicesBufferViewIndex = indicesAccessor["bufferView"];
    std::string indicesAccessorType = indicesAccessor["type"];
    uint32_t indicesAccessorComponentType = indicesAccessor["componentType"];
    if (indicesAccessorType != "SCALAR" || indicesAccessorComponentType != ComponentType::UNSIGNED_INT)
    {
        throw ModelParseError("Indices accessor is not a scalar unsigned integer");
    }

    nlohmann::json bufferViews = j["bufferViews"];

    nlohmann::json posBufferView = bufferViews[posBufferViewIndex];
    uint32_t posBufferIndex = posBufferView["buffer"];
    uint32_t posBufferSize = posBufferView["byteLength"];
    uint32_t posBufferOffset = 0;
    if (posBufferView.contains("byteOffset"))
    {
        posBufferOffset = posBufferView["byteOffset"];
    }

    nlohmann::json normBufferView = bufferViews[normBufferViewIndex];
    uint32_t normBufferIndex = normBufferView["buffer"];
    uint32_t normBufferSize = normBufferView["byteLength"];
    uint32_t normBufferOffset = 0;
    if (normBufferView.contains("byteOffset"))
    {
        normBufferOffset = normBufferView["byteOffset"];
    }
    if (posBufferSize != normBufferSize)
    {
        throw ModelParseError("Position buffer size is not equal to normal buffer size");
    }

    nlohmann::json indicesBufferView = bufferViews[indicesBufferViewIndex];
    uint32_t indicesBufferIndex = indicesBufferView["buffer"];
    uint32_t indicesBufferSize = indicesBufferView["byteLength"];
    uint32_t indicesBufferOffset = 0;
    if (indicesBufferView.contains("byteOffset"))
    {
        indicesBufferOffset = indicesBufferView["byteOffset"];
    }

    uint32_t binaryChunkLength = readU32LE((std::byte *)jsonChunkEnd);
    std::cout << "binaryChunkLength: " << binaryChunkLength << std::endl;
    uint32_t binaryChunkType = readU32LE((std::byte *)jsonChunkEnd + 4);
    if (binaryChunkType != ChunkType::BIN)
    {
        throw ModelParseError("Second chunk is not binary");
    }
    std::byte *binaryChunkStart = (std::byte *)jsonChunkEnd + 8;
    std::byte *posChunkStart = binaryChunkStart + posBufferOffset;
    std::byte *normChunkStart = binaryChunkStart + normBufferOffset;
    std::byte *indicesChunkStart = binaryChunkStart + indicesBufferOffset;

    ParsedData result;
    std::vector<Vertex> vertices;
    for (uint32_t i = 0; i < posBufferSize; i += 3 * sizeof(float))
    {
        Vertex v;
        Vec3 position{
            readFloatLE(posChunkStart + i),
            readFloatLE(posChunkStart + i + sizeof(float)),
            readFloatLE(posChunkStart + i + 2 * sizeof(float)),
        };
        v.position = position;
        Vec3 normal{
            readFloatLE(normChunkStart + i),
            readFloatLE(normChunkStart + i + sizeof(float)),
            readFloatLE(normChunkStart + i + 2 * sizeof(float)),
        };
        v.normal = normal;
        vertices.push_back(v);
    }
    result.vertices = vertices;

    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < indicesBufferSize; i += sizeof(uint32_t))
    {
        indices.push_back(readU32LE(indicesChunkStart + i));
    }
    result.indices = indices;

    return result;
}
