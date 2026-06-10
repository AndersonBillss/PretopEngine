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

ParsedData loadGlb(const std::string &path)
{
    std::vector<std::byte> bytes = readFileBytes(path);
    void *data = bytes.data();
    uint32_t magic = toUint32LE(*(uint32_t *)data);
    if(magic != 0x46546C67) {
        throw ModelParseError("GLB file corrupted");
    }
    return ParsedData();
}
