#pragma once
#include "appVertexBufferLayout.hpp"
#include "layoutType.hpp"
#include <vector>

class AppVertexLayout
{
public:
    AppVertexLayout(std::initializer_list<std::initializer_list<LayoutType>> layout);
    std::vector<AppVertexBufferLayout> bufferLayouts;
};