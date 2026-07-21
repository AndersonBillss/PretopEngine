#include "LoadGlb.hpp"
#include "ModelParseError.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

namespace Pretop::Asset
{
    enum ChunkType
    {
        Json = 0x4E4F534A,
        Bin = 0x004E4942,
    };

    enum ComponentType
    {
        Byte = 5120,
        UnsignedByte = 5121,
        Short = 5122,
        UnsignedShort = 5123,
        UnsignedInt = 5125,
        Float = 5126,
    };

    uint16_t ToUint16LE(uint16_t n)
    {
        uint8_t *bytes = reinterpret_cast<uint8_t *>(&n);
        return uint16_t(bytes[0]) |
               (uint16_t(bytes[1]) << 8);
    }
    uint16_t ReadU16LE(const std::byte *p)
    {
        uint16_t value;
        std::memcpy(&value, p, sizeof(value));
        return ToUint16LE(value);
    }
    uint32_t ToUint32LE(uint32_t n)
    {
        uint8_t *bytes = (uint8_t *)&n;
        return (uint32_t(bytes[0])) |
               (uint32_t(bytes[1]) << 8) |
               (uint32_t(bytes[2]) << 16) |
               (uint32_t(bytes[3]) << 24);
    }
    uint32_t ReadU32LE(const std::byte *p)
    {
        uint32_t value;
        std::memcpy(&value, p, sizeof(value));
        return ToUint32LE(value);
    }
    float ReadFloatLE(const std::byte *p)
    {
        uint32_t bits = ReadU32LE(p);

        float value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    std::vector<uint32_t> ReadIndicesU32(uint32_t indicesCount, const std::byte *indicesChunkStart)
    {
        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < indicesCount; i++)
        {
            uint32_t base = i * sizeof(uint32_t);
            indices.push_back(ReadU32LE(indicesChunkStart + base));
        }
        return indices;
    }
    std::vector<uint16_t> ReadIndicesU16(uint32_t indicesCount, const std::byte *indicesChunkStart)
    {
        std::vector<uint16_t> indices;
        for (uint32_t i = 0; i < indicesCount; i++)
        {
            uint32_t base = i * sizeof(uint16_t);
            indices.push_back(ReadU16LE(indicesChunkStart + base));
        }
        return indices;
    }

    ParsedData LoadGlb(const AssetBytes &bytes)
    {
        const std::byte *data = bytes.data();
        uint32_t magic = ReadU32LE(data);
        if (magic != 0x46546C67)
        {
            throw ModelParseError("GLB file corrupted");
        }
        uint32_t version = ReadU32LE(data + 4);
        // std::cout << "Version: " << version << std::endl;
        uint32_t length = ReadU32LE(data + 8);
        uint32_t jsonChunkLength = ReadU32LE(data + 12);
        uint32_t jsonChunkType = ReadU32LE(data + 16);
        if (jsonChunkType != ChunkType::Json)
        {
            throw ModelParseError("First chunk is not JSON");
        }
        // std::cout << "Chunk length: " << jsonChunkLength << std::endl;
        const char *jsonChunkStart = reinterpret_cast<const char *>(data + 20);
        const char *jsonChunkEnd = jsonChunkStart + jsonChunkLength;
        nlohmann::json j = nlohmann::json::parse(jsonChunkStart, jsonChunkEnd);
        // std::cout << j.dump(2) << std::endl;

        nlohmann::json mesh = j["meshes"][0];
        nlohmann::json primitives = mesh["primitives"];
        nlohmann::json primitive = primitives[0];
        uint32_t posIndex = primitive["attributes"]["POSITION"];
        // std::cout << "posIndex: " << posIndex << std::endl;
        uint32_t normIndex = primitive["attributes"]["NORMAL"];
        // std::cout << "normIndex: " << normIndex << std::endl;
        uint32_t indicesIndex = primitive["indices"];
        // std::cout << "indicesIndex: " << indicesIndex << std::endl;

        nlohmann::json accessors = j["accessors"];
        nlohmann::json posAccessor = accessors[posIndex];
        std::string posAccessorType = posAccessor["type"];
        uint32_t posCount = posAccessor["count"];
        uint32_t posAccessorComponentType = posAccessor["componentType"];
        if (posAccessorType != "VEC3" || posAccessorComponentType != ComponentType::Float)
        {
            throw ModelParseError("Position accessor is not a 3D vector of floats");
        }
        uint32_t posBufferViewIndex = posAccessor["bufferView"];

        nlohmann::json normAccessor = accessors[normIndex];
        std::string normAccessorType = normAccessor["type"];
        uint32_t normAccessorComponentType = normAccessor["componentType"];
        uint32_t normCount = normAccessor["count"];
        if (normAccessorType != "VEC3" || normAccessorComponentType != ComponentType::Float)
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
            indicesAccessorComponentType != ComponentType::UnsignedInt &&
            indicesAccessorComponentType != ComponentType::UnsignedShort)
        {
            throw ModelParseError("Indices accessor is not an unsigned integer or an unsigned short");
        }
        uint8_t indicesComponentSize = indicesAccessorComponentType ==
                                               ComponentType::UnsignedInt
                                           ? sizeof(uint32_t)
                                           : sizeof(uint16_t);

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
        // std::cout << "posBufferOffset: " << posBufferOffset << std::endl;
        // std::cout << "normBufferOffset: " << normBufferOffset << std::endl;

        nlohmann::json indicesBufferView = bufferViews[indicesBufferViewIndex];
        uint32_t indicesBufferIndex = indicesBufferView["buffer"];
        uint32_t indicesBufferOffset = 0;
        if (indicesBufferView.contains("byteOffset"))
        {
            indicesBufferOffset = indicesBufferView["byteOffset"];
        }

        const auto *jsonChunkBytes = reinterpret_cast<const std::byte *>(jsonChunkEnd);
        uint32_t binaryChunkLength = ReadU32LE(jsonChunkBytes);
        // std::cout << "binaryChunkLength: " << binaryChunkLength << std::endl;
        uint32_t binaryChunkType = ReadU32LE(jsonChunkBytes + 4);
        if (binaryChunkType != ChunkType::Bin)
        {
            throw ModelParseError("Second chunk is not binary");
        }
        const std::byte *binaryChunkStart = jsonChunkBytes + 8;
        const std::byte *posChunkStart = binaryChunkStart + posBufferOffset;
        const std::byte *normChunkStart = binaryChunkStart + normBufferOffset;
        const std::byte *indicesChunkStart = binaryChunkStart + indicesBufferOffset;

        ParsedData result;
        std::vector<Vertex> vertices;
        for (uint32_t i = 0; i < posCount; i++)
        {
            uint32_t base = i * 3 * sizeof(float);
            Vertex v;
            v.Position = {
                ReadFloatLE(posChunkStart + base + 0),
                ReadFloatLE(posChunkStart + base + 4),
                ReadFloatLE(posChunkStart + base + 8),
            };
            v.Normal = {
                ReadFloatLE(normChunkStart + base + 0),
                ReadFloatLE(normChunkStart + base + 4),
                ReadFloatLE(normChunkStart + base + 8),
            };
            vertices.push_back(v);
        }
        result.Vertices = vertices;

        if (indicesAccessorComponentType == ComponentType::UnsignedInt)
        {
            std::vector<uint32_t> indicesU32 = ReadIndicesU32(indicesCount, indicesChunkStart);
            IndexList indices{std::in_place_type<std::vector<uint32_t>>, indicesU32};
            result.Indices = indices;
        }
        else
        {
            std::vector<uint16_t> indicesU16 = ReadIndicesU16(indicesCount, indicesChunkStart);
            IndexList indices{std::in_place_type<std::vector<uint16_t>>, indicesU16};
            result.Indices = indices;
        }

        return result;
    }
} // namespace Pretop::Asset
