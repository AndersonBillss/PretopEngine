#pragma once
#include <memory>
#include <webgpu/webgpu.h>
#include <functional>
#include <initializer_list>
#include "../Window/Window.hpp"
#include "Adapter.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "CommandBuffer.hpp"
#include "Buffer.hpp"

class Application
{
public:
    using TickCallback = std::function<void(double dt, WGPUTextureView targetView)>;
    using StartupCallback = std::function<void(Application &app)>;

    Application();

    void initialize(StartupCallback cb);
    void run(TickCallback cb);

    void writeBufZero(const AppBuffer &buf)
    {
        std::vector<uint8_t> data = buf.zeroBuffer();
        wgpuQueueWriteBuffer(
            this->_queue,
            buf.wgpuBuffer,
            0,
            data.data(),
            data.size());
    }
    void writeBuf(const AppBuffer &buf, void *data, size_t numBytes)
    {
        wgpuQueueWriteBuffer(
            this->_queue,
            buf.wgpuBuffer,
            0,
            data,
            numBytes);
    }

    template <class T>
    void writeBuf(const AppBuffer &buf, T &data)
    {
        wgpuQueueWriteBuffer(
            this->_queue,
            buf.wgpuBuffer,
            0,
            &data,
            sizeof(T));
    }

    template <class T>
    void writeVec(const AppBuffer &buf, std::vector<T> &vec)
    {
        wgpuQueueWriteBuffer(
            this->_queue,
            buf.wgpuBuffer,
            0,
            vec.data(),
            vec.size() * sizeof(T));
    }

    void submitCommandBuffer(AppCommandBuffer &buf);
    void setWindow(std::unique_ptr<Window> win);
    void logQueueCommands();
    WGPUTextureView getNextSurfaceTextureView();
    WGPUTextureFormat windowFormat;

    Application *inspectInstance();
    Application *inspectDevice();
    Application *inspectAdapter();
    Application *inspectQueue();

    std::unique_ptr<AppInstance> instance;

private:
    void _createQueue();

    bool _logQueueCommands;
    std::unique_ptr<AppAdapter> _adapter;
    WGPUQueue _queue;
    WGPUSurface _windowSurface;
    std::unique_ptr<Window> _window;

public:
    // This must go after the private section or else the initializer
    // list order will order the constructors incorrectly
    std::unique_ptr<AppDevice> device;
};
