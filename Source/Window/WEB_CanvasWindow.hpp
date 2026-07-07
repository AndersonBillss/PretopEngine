#pragma once
#include "Window.hpp"
#include <string>
#include <chrono>

class CanvasWindow : public Window
{
public:
    ~CanvasWindow() override;
    CanvasWindow(unsigned int width, unsigned int height, std::string title);
    CanvasWindow(std::string title);

    void SetOnTick(Window::TickCallback callback) override;
    void SetOnExit(Window::ExitCallback callback) override;
    void Run() override;
    bool IsInitialized() override;
    WGPUSurface GetSurface(WGPUInstance instance) override;

private:
    Window::TickCallback _onTick;
    Window::ExitCallback _onExit;
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> _lastFrame;
};