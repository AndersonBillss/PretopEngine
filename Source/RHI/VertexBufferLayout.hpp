#pragma once
#include <initializer_list>
#include <vector>
#include <webgpu/webgpu.h>
#include "LayoutType.hpp"

namespace Pretop::RHI
{
    class AppVertexBufferLayout
    {
    public:
        AppVertexBufferLayout(std::initializer_list<LayoutType> shape, size_t startLayout);
        std::vector<WGPUVertexAttribute> WgpuAttrs;
        size_t ArrayStride;
        size_t EndLayout;

    private:
        WGPUVertexFormat GetFmt(LayoutType attrType);
        size_t GetSize(LayoutType numType);
    };
} // namespace Pretop::RHI
