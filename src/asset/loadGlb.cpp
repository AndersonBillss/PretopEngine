#include "loadGlb.hpp"
#include "assetLoaderFactory.hpp"
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

uint16_t toUint16LE(uint16_t n)
{
    uint8_t *bytes = reinterpret_cast<uint8_t *>(&n);
    return uint16_t(bytes[0]) |
           (uint16_t(bytes[1]) << 8);
}
uint16_t readU16LE(const std::byte *p)
{
    uint16_t value;
    std::memcpy(&value, p, sizeof(value));
    return toUint16LE(value);
}
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

std::vector<uint32_t> readIndicesU32(uint32_t indicesCount, std::byte *indicesChunkStart)
{
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < indicesCount; i++)
    {
        uint32_t base = i * sizeof(uint32_t);
        indices.push_back(readU32LE(indicesChunkStart + base));
    }
    return indices;
}
std::vector<uint16_t> readIndicesU16(uint32_t indicesCount, std::byte *indicesChunkStart)
{
    std::vector<uint16_t> indices;
    for (uint32_t i = 0; i < indicesCount; i++)
    {
        uint32_t base = i * sizeof(uint16_t);
        indices.push_back(readU16LE(indicesChunkStart + base));
    }
    return indices;
}

ParsedData loadGlb(const std::string &path)
{
    auto assetLoader = AssetLoaderFactory::createAssetLoader();
    auto handle = assetLoader->loadBinaryAsync(path);
    handle.wait();
    auto handleResult = handle.get();
    if(!handleResult) {
        throw ModelParseError("Asset could not be loaded: " + handleResult.error);
    }
    std::byte *data = handleResult.data.data();
    std::cout << "HERE!!!1" << std::endl;
    uint32_t magic = readU32LE(data);
    std::cout << "HERE!!!2" << std::endl;
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
    std::cout << j.dump(2) << std::endl;

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
    uint32_t posCount = posAccessor["count"];
    uint32_t posAccessorComponentType = posAccessor["componentType"];
    if (posAccessorType != "VEC3" || posAccessorComponentType != ComponentType::FLOAT)
    {
        throw ModelParseError("Position accessor is not a 3D vector of floats");
    }
    uint32_t posBufferViewIndex = posAccessor["bufferView"];

    nlohmann::json normAccessor = accessors[normIndex];
    std::string normAccessorType = normAccessor["type"];
    uint32_t normAccessorComponentType = normAccessor["componentType"];
    uint32_t normCount = normAccessor["count"];
    if (normAccessorType != "VEC3" || normAccessorComponentType != ComponentType::FLOAT)
    {
        throw ModelParseError("Normal accessor is not a 3D vector of floats");
    }
    uint32_t normBufferViewIndex = normAccessor["bufferView"];

    if (posCount != normCount)
    {
        throw ModelParseError("Position count is not equal to normal buffer count");
    }

    nlohmann::json indicesAccessor = accessors[indicesIndex];
    uint32_t indicesBufferViewIndex = indicesAccessor["bufferView"];
    std::string indicesAccessorType = indicesAccessor["type"];
    uint32_t indicesAccessorComponentType = indicesAccessor["componentType"];
    uint32_t indicesCount = indicesAccessor["count"];
    if (indicesAccessorType != "SCALAR")
    {
        throw ModelParseError("Indices accessor is not a scalar");
    }
    if (
        indicesAccessorComponentType != ComponentType::UNSIGNED_INT &&
        indicesAccessorComponentType != ComponentType::UNSIGNED_SHORT)
    {
        throw ModelParseError("Indices accessor is not an unsigned integer or an unsigned short");
    }
    uint8_t indicesComponentSize = indicesAccessorComponentType ==
                                           ComponentType::UNSIGNED_INT
                                       ? sizeof(UNSIGNED_INT)
                                       : sizeof(UNSIGNED_SHORT);

    nlohmann::json bufferViews = j["bufferViews"];

    nlohmann::json posBufferView = bufferViews[posBufferViewIndex];
    uint32_t posBufferIndex = posBufferView["buffer"];
    uint32_t posBufferOffset = 0;
    if (posBufferView.contains("byteOffset"))
    {
        posBufferOffset = posBufferView["byteOffset"];
    }

    nlohmann::json normBufferView = bufferViews[normBufferViewIndex];
    uint32_t normBufferIndex = normBufferView["buffer"];
    uint32_t normBufferOffset = 0;
    if (normBufferView.contains("byteOffset"))
    {
        normBufferOffset = normBufferView["byteOffset"];
    }
    std::cout << "posBufferOffset: " << posBufferOffset << std::endl;
    std::cout << "normBufferOffset: " << normBufferOffset << std::endl;

    nlohmann::json indicesBufferView = bufferViews[indicesBufferViewIndex];
    uint32_t indicesBufferIndex = indicesBufferView["buffer"];
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
    for (uint32_t i = 0; i < posCount; i++)
    {
        uint32_t base = i * 3 * sizeof(float);
        Vertex v;
        v.position = {
            readFloatLE(posChunkStart + base + 0),
            readFloatLE(posChunkStart + base + 4),
            readFloatLE(posChunkStart + base + 8),
        };
        v.normal = {
            readFloatLE(normChunkStart + base + 0),
            readFloatLE(normChunkStart + base + 4),
            readFloatLE(normChunkStart + base + 8),
        };
        vertices.push_back(v);
    }
    result.vertices = vertices;

    if (indicesAccessorComponentType == ComponentType::UNSIGNED_INT)
    {
        std::vector<uint32_t> indicesU32 = readIndicesU32(indicesCount, indicesChunkStart);
        IndexList indices{std::in_place_type<std::vector<uint32_t>>, indicesU32};
        result.indices = indices;
    }
    else
    {
        std::vector<uint16_t> indicesU16 = readIndicesU16(indicesCount, indicesChunkStart);
        IndexList indices{std::in_place_type<std::vector<uint16_t>>, indicesU16};
        result.indices = indices;
    }

    return result;
}
