#pragma once
#include <initializer_list>
#include <vector>
#include <webgpu/webgpu.h>

#include "LayoutType.hpp"

class AppVertexBufferLayout
{
public:
    AppVertexBufferLayout(std::initializer_list<LayoutType> shape, size_t startLayout);
    std::vector<WGPUVertexAttribute> wgpuAttrs;
    size_t arrayStride;
    size_t endLayout;

private:
    WGPUVertexFormat getFmt(LayoutType attrType);
    size_t getSize(LayoutType numType);
};