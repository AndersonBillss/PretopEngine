#include "VertexBufferLayout.hpp"
#include <stdexcept>
#include <unordered_map>

namespace Pretop::RHI
{
    VertexBufferLayout::VertexBufferLayout(std::initializer_list<LayoutType> shape, size_t startLayout = 0)
    {
        size_t i = startLayout;
        size_t totalOffset = 0;
        for (const auto &attrShape : shape)
        {
            WGPUVertexAttribute attr = WGPU_VERTEX_ATTRIBUTE_INIT;
            attr.shaderLocation = i;
            attr.format = GetFmt(attrShape);
            attr.offset = totalOffset;
            totalOffset += GetSize(attrShape);
            WgpuAttrs.push_back(attr);
            i++;
        }
        this->EndLayout = i;
        this->ArrayStride = totalOffset;
    };

    WGPUVertexFormat VertexBufferLayout::GetFmt(LayoutType attrType)
    {
        const std::unordered_map<size_t, WGPUVertexFormat> toWGUFmt = {
            {Uint8, WGPUVertexFormat_Uint8},
            {Uint8x2, WGPUVertexFormat_Uint8x2},
            {Uint8x4, WGPUVertexFormat_Uint8x4},
            {Sint8, WGPUVertexFormat_Sint8},
            {Sint8x2, WGPUVertexFormat_Sint8x2},
            {Sint8x4, WGPUVertexFormat_Sint8x4},
            {Unorm8, WGPUVertexFormat_Unorm8},
            {Unorm8x2, WGPUVertexFormat_Unorm8x2},
            {Unorm8x4, WGPUVertexFormat_Unorm8x4},
            {Snorm8, WGPUVertexFormat_Snorm8},
            {Snorm8x2, WGPUVertexFormat_Snorm8x2},
            {Snorm8x4, WGPUVertexFormat_Snorm8x4},
            {Uint16, WGPUVertexFormat_Uint16},
            {Uint16x2, WGPUVertexFormat_Uint16x2},
            {Uint16x4, WGPUVertexFormat_Uint16x4},
            {Sint16, WGPUVertexFormat_Sint16},
            {Sint16x2, WGPUVertexFormat_Sint16x2},
            {Sint16x4, WGPUVertexFormat_Sint16x4},
            {Unorm16, WGPUVertexFormat_Unorm16},
            {Unorm16x2, WGPUVertexFormat_Unorm16x2},
            {Unorm16x4, WGPUVertexFormat_Unorm16x4},
            {Snorm16, WGPUVertexFormat_Snorm16},
            {Snorm16x2, WGPUVertexFormat_Snorm16x2},
            {Snorm16x4, WGPUVertexFormat_Snorm16x4},
            {Float16, WGPUVertexFormat_Float16},
            {Float16x2, WGPUVertexFormat_Float16x2},
            {Float16x4, WGPUVertexFormat_Float16x4},
            {Float32, WGPUVertexFormat_Float32},
            {Float32x2, WGPUVertexFormat_Float32x2},
            {Float32x3, WGPUVertexFormat_Float32x3},
            {Float32x4, WGPUVertexFormat_Float32x4},
            {Uint32, WGPUVertexFormat_Uint32},
            {Uint32x2, WGPUVertexFormat_Uint32x2},
            {Uint32x3, WGPUVertexFormat_Uint32x3},
            {Uint32x4, WGPUVertexFormat_Uint32x4},
            {Sint32, WGPUVertexFormat_Sint32},
            {Sint32x2, WGPUVertexFormat_Sint32x2},
            {Sint32x3, WGPUVertexFormat_Sint32x3},
            {Sint32x4, WGPUVertexFormat_Sint32x4},
        };
        auto it = toWGUFmt.find(attrType);
        if (it == toWGUFmt.end())
        {
            throw std::invalid_argument("Invalid vertex buffer shape");
        }
        return it->second;
    }

    size_t VertexBufferLayout::GetSize(LayoutType numType)
    {
        const std::unordered_map<size_t, size_t> nTypeToSize = {
            {Uint8, 1 * 1},
            {Uint8x2, 1 * 2},
            {Uint8x4, 1 * 4},
            {Sint8, 1 * 1},
            {Sint8x2, 1 * 2},
            {Sint8x4, 1 * 4},
            {Unorm8, 1 * 1},
            {Unorm8x2, 1 * 2},
            {Unorm8x4, 1 * 4},
            {Snorm8, 1 * 1},
            {Snorm8x2, 1 * 2},
            {Snorm8x4, 1 * 4},
            {Uint16, 2 * 1},
            {Uint16x2, 2 * 2},
            {Uint16x4, 2 * 4},
            {Sint16, 2 * 1},
            {Sint16x2, 2 * 2},
            {Sint16x4, 2 * 4},
            {Unorm16, 2 * 1},
            {Unorm16x2, 2 * 2},
            {Unorm16x4, 2 * 4},
            {Snorm16, 2 * 1},
            {Snorm16x2, 2 * 2},
            {Snorm16x4, 2 * 4},
            {Float16, 2 * 1},
            {Float16x2, 2 * 2},
            {Float16x4, 2 * 4},
            {Float32, 4 * 1},
            {Float32x2, 4 * 2},
            {Float32x3, 4 * 3},
            {Float32x4, 4 * 4},
            {Uint32, 4 * 1},
            {Uint32x2, 4 * 2},
            {Uint32x3, 4 * 3},
            {Uint32x4, 4 * 4},
            {Sint32, 4 * 1},
            {Sint32x2, 4 * 2},
            {Sint32x3, 4 * 3},
            {Sint32x4, 4 * 4},
        };

        auto it = nTypeToSize.find(numType);
        if (it == nTypeToSize.end())
        {
            throw std::invalid_argument("Invalid vertex buffer shape");
        }
        return it->second;
    }
} // namespace Pretop::RHI
