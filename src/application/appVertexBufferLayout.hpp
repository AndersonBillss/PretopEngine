#pragma once
#include <initializer_list>
#include <vector>
#include <webgpu/webgpu.h>

enum NumType
{
    Default = 0,
    Uint8,
    int8,
    Sint8,
    Unorm8,
    Snorm8,
    Uint16,
    Sint16,
    Unorm16,
    Snorm16,
    Float16,
    Float32,
    Uint32,
    Sint32,
};
class AppVertexBufferLayout
{
public:
    AppVertexBufferLayout(std::initializer_list<std::initializer_list<unsigned char>> shape);
    std::vector<WGPUVertexAttribute> wgpuAttrs;
    size_t arrayStride;

private:
    WGPUVertexFormat getFmt(unsigned char attrType, size_t attrSize);
    size_t getSize(size_t numType);
};