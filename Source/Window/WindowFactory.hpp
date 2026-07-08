#pragma once
#include <memory>
#include <string>
#include "Window.hpp"

namespace Pretop::Window
{
    namespace WindowFactory
    {
        std::unique_ptr<Window> CreateWindow(int width, int height, std::string title);
        std::unique_ptr<Window> CreateWindow(std::string title);
    }
} // namespace Pretop::Window
