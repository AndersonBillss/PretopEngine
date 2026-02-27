#pragma once
#include <string>
#include <functional>
#include <webgpu/webgpu.h>

class Window
{
public:
    using TickCallback = std::function<void(double dt)>;
    using ExitCallback = std::function<void()>;

    virtual ~Window() = default;
    Window(int width, int height, std::string title)
        : width(width), height(height), title_(std::move(title)) {}

    Window(std::string title)
        : Window(default_width, default_height, std::move(title)) {}

    virtual bool isInitialized() = 0;
    virtual void setOnTick(TickCallback cb) = 0;
    virtual void setOnExit(ExitCallback cb) = 0;
    virtual void run() = 0;
    virtual WGPUSurface getSurface(WGPUInstance instance) = 0;

    int width = 0;
    int height = 0;

protected:
    std::string title_;
    static constexpr int default_width = 800;
    static constexpr int default_height = 600;
};
