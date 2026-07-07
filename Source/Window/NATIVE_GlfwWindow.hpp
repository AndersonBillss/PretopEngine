#pragma once
#include "Window.hpp"
#include <GLFW/glfw3.h>
#include <string>

class GlfwWindow : public Window
{
public:
    ~GlfwWindow() override;
    GlfwWindow(unsigned int width, unsigned int height, std::string title);
    GlfwWindow(std::string title);

    void SetOnTick(Window::TickCallback callback) override;
    void SetOnExit(Window::ExitCallback callback) override;
    void Run() override;
    bool IsInitialized() override;
    WGPUSurface GetSurface(WGPUInstance instance) override;

private:
    Window::TickCallback _onTick;
    Window::ExitCallback _onExit;
    GLFWwindow *_window;
};