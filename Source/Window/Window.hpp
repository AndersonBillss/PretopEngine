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
    Window(unsigned int width, unsigned int height, std::string title)
        : Width(width), Height(height), _title(std::move(title)) {}

    Window(std::string title)
        : Window(DefaultWidth, DefaultHeight, std::move(title)) {}

    virtual bool IsInitialized() = 0;
    virtual void SetOnTick(TickCallback callback) = 0;
    virtual void SetOnExit(ExitCallback callback) = 0;
    virtual void Run() = 0;
    virtual WGPUSurface GetSurface(WGPUInstance instance) = 0;

    unsigned int Width = 0;
    unsigned int Height = 0;

protected:
    std::string _title;
    static constexpr int DefaultWidth = 800;
    static constexpr int DefaultHeight = 600;
};
