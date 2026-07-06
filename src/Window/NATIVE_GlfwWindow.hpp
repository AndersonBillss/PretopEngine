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

    void setOnTick(Window::TickCallback cb) override;
    void setOnExit(Window::ExitCallback cb) override;
    void run() override;
    bool isInitialized() override;
    WGPUSurface getSurface(WGPUInstance instance) override;

private:
    Window::TickCallback _onTick;
    Window::ExitCallback _onExit;
    GLFWwindow *_win;
};