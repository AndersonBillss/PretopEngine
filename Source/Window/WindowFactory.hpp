#pragma once
#include <memory>
#include <string>
#include "Window.hpp"

namespace WindowFactory
{
    std::unique_ptr<Window> createWindow(int width, int height, std::string title);
    std::unique_ptr<Window> createWindow(std::string title);
}