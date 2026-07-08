#include "VertexLayout.hpp"
#include <iostream>

namespace Pretop::RHI
{
    AppVertexLayout::AppVertexLayout(std::initializer_list<std::initializer_list<LayoutType>> layout)
    {
        size_t startLayout = 0;
        for (const auto &bufLayoutRaw : layout)
        {
            AppVertexBufferLayout bufLayout(bufLayoutRaw, startLayout);
            BufferLayouts.push_back(bufLayout);
            startLayout = bufLayout.EndLayout;
        }
    }
} // namespace Pretop::RHI
