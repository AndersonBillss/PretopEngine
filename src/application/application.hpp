#pragma once
#include <memory>
#include <webgpu/webgpu.h>
#include <functional>
#include <initializer_list>
#include "../window/window.hpp"
#include "appAdapter.hpp"
#include "appDevice.hpp"
#include "appInstance.hpp"
#include "appCommandBuffer.hpp"
#include "appBuffer.hpp"

class Application
{
public:
    using TickCallback = std::function<void(double dt, WGPUTextureView targetView)>;

    Application();

    void run(TickCallback cb);

    void writeVertices(const std::initializer_list<AppBuffer<float> *> &bufs);
    void writeIndex(const AppBuffer<uint16_t> &buf);
    void writeIndex(const AppBuffer<uint32_t> &buf);
    template <class T>
    void writeBuf(const AppBuffer<T> &buf)
    {
        wgpuQueueWriteBuffer(
            this->_queue,
            buf.wgpuBuffer,
            0,
            buf.data(),
            buf.numBytes());
    };
    void submitCommandBuffer(AppCommandBuffer &buf);
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
