#pragma once
#include "VertexBufferLayout.hpp"
#include "LayoutType.hpp"
#include <vector>

class AppVertexLayout
{
public:
    AppVertexLayout(std::initializer_list<std::initializer_list<LayoutType>> layout);
    std::vector<AppVertexBufferLayout> bufferLayouts;
};