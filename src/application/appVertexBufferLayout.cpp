#include "appVertexBufferLayout.hpp"
#include <stdexcept>
#include <unordered_map>

AppVertexBufferLayout::AppVertexBufferLayout(std::initializer_list<std::initializer_list<unsigned char>> shape, size_t startLayout)
{
    size_t i = startLayout;
    size_t totalOffset = 0;
    for (const auto &attrShape : shape)
    {
        size_t numType = 0;
        size_t attrSize = attrShape.size();
        for (size_t n : attrShape)
        {
            numType = n;
            break;
        }
        if (numType == 0)
        {
            throw std::invalid_argument("Invalid vertex buffer shape");
        }
        WGPUVertexAttribute attr = WGPU_VERTEX_ATTRIBUTE_INIT;
        attr.shaderLocation = i;
        attr.format = getFmt(numType, attrSize);
        attr.offset = totalOffset;
        totalOffset += getSize(numType) * attrSize;
        wgpuAttrs.push_back(attr);
        i++;
    }
    this->endLayout = i;
    this->arrayStride = totalOffset;
}

WGPUVertexFormat AppVertexBufferLayout::getFmt(unsigned char attrType, size_t attrSize)
{
    const std::unordered_map<size_t, WGPUVertexFormat> toFormat = {
        {NumType::Sint8 + (1 << 8), WGPUVertexFormat_Sint8},
        {NumType::Sint8 + (2 << 8), WGPUVertexFormat_Sint8x2},
        {NumType::Sint8 + (4 << 8), WGPUVertexFormat_Sint8x4},
        {NumType::Unorm8 + (1 << 8), WGPUVertexFormat_Unorm8},
        {NumType::Unorm8 + (2 << 8), WGPUVertexFormat_Unorm8x2},
        {NumType::Unorm8 + (4 << 8), WGPUVertexFormat_Unorm8x4},
        {NumType::Snorm8 + (1 << 8), WGPUVertexFormat_Snorm8},
        {NumType::Snorm8 + (2 << 8), WGPUVertexFormat_Snorm8x2},
        {NumType::Snorm8 + (4 << 8), WGPUVertexFormat_Snorm8x4},
        {NumType::Uint16 + (1 << 8), WGPUVertexFormat_Uint16},
        {NumType::Uint16 + (2 << 8), WGPUVertexFormat_Uint16x2},
        {NumType::Uint16 + (4 << 8), WGPUVertexFormat_Uint16x4},
        {NumType::Sint16 + (1 << 8), WGPUVertexFormat_Sint16},
        {NumType::Sint16 + (2 << 8), WGPUVertexFormat_Sint16x2},
        {NumType::Sint16 + (4 << 8), WGPUVertexFormat_Sint16x4},
        {NumType::Unorm16 + (1 << 8), WGPUVertexFormat_Unorm16},
        {NumType::Unorm16 + (2 << 8), WGPUVertexFormat_Unorm16x2},
        {NumType::Unorm16 + (4 << 8), WGPUVertexFormat_Unorm16x4},
        {NumType::Snorm16 + (1 << 8), WGPUVertexFormat_Snorm16},
        {NumType::Snorm16 + (2 << 8), WGPUVertexFormat_Snorm16x2},
        {NumType::Snorm16 + (4 << 8), WGPUVertexFormat_Snorm16x4},
        {NumType::Float16 + (1 << 8), WGPUVertexFormat_Float16},
        {NumType::Float16 + (2 << 8), WGPUVertexFormat_Float16x2},
        {NumType::Float16 + (4 << 8), WGPUVertexFormat_Float16x4},
        {NumType::Float32 + (1 << 8), WGPUVertexFormat_Float32},
        {NumType::Float32 + (2 << 8), WGPUVertexFormat_Float32x2},
        {NumType::Float32 + (3 << 8), WGPUVertexFormat_Float32x3},
        {NumType::Float32 + (4 << 8), WGPUVertexFormat_Float32x4},
        {NumType::Uint32 + (1 << 8), WGPUVertexFormat_Uint32},
        {NumType::Uint32 + (2 << 8), WGPUVertexFormat_Uint32x2},
        {NumType::Uint32 + (3 << 8), WGPUVertexFormat_Uint32x3},
        {NumType::Uint32 + (4 << 8), WGPUVertexFormat_Uint32x4},
        {NumType::Sint32 + (1 << 8), WGPUVertexFormat_Sint32},
        {NumType::Sint32 + (2 << 8), WGPUVertexFormat_Sint32x2},
        {NumType::Sint32 + (3 << 8), WGPUVertexFormat_Sint32x3},
        {NumType::Sint32 + (4 << 8), WGPUVertexFormat_Sint32x4},
    };

    WGPUVertexAttribute attr = WGPU_VERTEX_ATTRIBUTE_INIT;
    auto it = toFormat.find(attrType + (attrSize << 8));
    if (it == toFormat.end())
    {
        throw std::invalid_argument("Invalid vertex buffer shape");
    }
    return it->second;
}

size_t AppVertexBufferLayout::getSize(size_t numType)
{
    const std::unordered_map<size_t, size_t> nTypeToSize = {
        {Uint8, 1},
        {int8, 1},
        {Sint8, 1},
        {Unorm8, 1},
        {Snorm8, 1},
        {Uint16, 2},
        {Sint16, 2},
        {Unorm16, 2},
        {Snorm16, 2},
        {Float16, 2},
        {Float32, 4},
        {Uint32, 4},
        {Sint32, 4},
    };
    return nTypeToSize.find(numType)->second;
}
