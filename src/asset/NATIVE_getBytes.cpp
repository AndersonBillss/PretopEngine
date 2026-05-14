#include "NATIVE_getBytes.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>

std::vector<std::byte> readFileBytes(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::streamsize size = file.tellg();
    if (size < 0)
    {
        throw std::runtime_error("Failed to get file size: " + path);
    }

    std::vector<std::byte> bytes(static_cast<size_t>(size));

    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char *>(bytes.data()), size))
    {
        throw std::runtime_error("Failed to read file: " + path);
    }

    return bytes;
}

std::future<std::vector<std::byte>> getBytes(const std::string &path)
{
    return std::async(std::launch::async, [path]
                      { return readFileBytes(path); });
}
