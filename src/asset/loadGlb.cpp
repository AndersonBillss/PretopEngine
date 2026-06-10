#include "loadGlb.hpp"
#include "readFileBytes.hpp"
#include "modelParseError.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

uint32_t toUint32LE(uint32_t n)
{
    uint8_t *bytes = (uint8_t *)&n;
    return (uint32_t(bytes[0])) |
           (uint32_t(bytes[1]) << 8) |
           (uint32_t(bytes[2]) << 16) |
           (uint32_t(bytes[3]) << 24);
}
uint32_t readU32LE(const std::byte* p)
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
    if(magic != 0x46546C67) {
        throw ModelParseError("GLB file corrupted");
    }
    uint32_t version = readU32LE(data + 4);
    std::cout << "Version: " << version << std::endl;
    uint32_t length = readU32LE(data + 8);
    uint32_t chunkLength = readU32LE(data + 12);
    std::cout << "Chunk length: " << chunkLength << std::endl;
    char *chunkStart = (char *)(data + 20);
    nlohmann::json j = nlohmann::json::parse(chunkStart , chunkStart + chunkLength);
    std::cout << j.dump(2) << std::endl;
    return ParsedData();
}
