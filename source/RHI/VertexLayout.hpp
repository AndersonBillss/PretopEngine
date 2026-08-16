#pragma once
#include "VertexBufferLayout.hpp"
#include "LayoutType.hpp"
#include <vector>

namespace Pretop::RHI
{
    class VertexLayout
    {
    public:
        VertexLayout(std::initializer_list<std::initializer_list<LayoutType>> layout);
        std::vector<VertexBufferLayout> BufferLayouts;
    };
} // namespace Pretop::RHI
