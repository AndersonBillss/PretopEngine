#pragma once
#include "window.hpp"
#include <string>
#include <chrono>

class CanvasWindow : public Window
{
public:
    ~CanvasWindow() override;
    CanvasWindow(unsigned int width, unsigned int height, std::string title);
    CanvasWindow(std::string title);

    void setOnTick(Window::TickCallback cb) override;
    void setOnExit(Window::ExitCallback cb) override;
    void run() override;
    bool isInitialized() override;
    WGPUSurface getSurface(WGPUInstance instance) override;

private:
    Window::TickCallback _onTick;
    Window::ExitCallback _onExit;
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> _lastFrame;
};