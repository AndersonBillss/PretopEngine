#pragma once
#include <memory>
#include <webgpu/webgpu.h>
#include <functional>
#include "../window/window.hpp"
#include "appAdapter.hpp"
#include "appDevice.hpp"
#include "appInstance.hpp"
#include "appCommandBuffer.hpp"
#include "appVertexBuffer.hpp"

class Application
{
public:
    using TickCallback = std::function<AppCommandBuffer(double dt, WGPUTextureView targetView)>;

    Application();

    void run(TickCallback cb);

    void writeVertices(AppVertexBuffer<float> &buf);
    void setWindow(std::unique_ptr<Window> win);
    void logQueueCommands();
    WGPUTextureView getNextSurfaceTextureView();
    WGPUTextureFormat windowFormat;

    Application *inspectInstance();
    Application *inspectDevice();
    Application *inspectAdapter();
    Application *inspectQueue();

    AppInstance instance;

private:
    void _createQueue();

    bool _logQueueCommands;
    AppAdapter _adapter;
    WGPUQueue _queue;
    WGPUSurface _windowSurface;
    std::unique_ptr<Window> _window;

public:
    // This must go after the private section or else the initializer
    // list order will order the constructors incorrectly
    AppDevice device;
};
