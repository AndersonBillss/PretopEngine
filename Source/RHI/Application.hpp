#pragma once
#include <memory>
#include <webgpu/webgpu.h>
#include <functional>
#include <initializer_list>
#include <vector>
#include "../Window/Window.hpp"
#include "Adapter.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "CommandBuffer.hpp"
#include "Buffer.hpp"

namespace Pretop::RHI
{
    class Application
    {
    public:
        using TickCallback = std::function<void(double dt, WGPUTextureView targetView)>;
        using StartupCallback = std::function<void(Application &app)>;

        static void Create(StartupCallback callback);
        void Run(TickCallback callback);

        void WriteBufZero(const AppBuffer &buf)
        {
            std::vector<uint8_t> data = buf.ZeroBuffer();
            wgpuQueueWriteBuffer(
                this->_queue,
                buf.WgpuBuffer,
                0,
                data.data(),
                data.size());
        }
        void WriteBuf(const AppBuffer &buf, void *data, size_t numBytes)
        {
            wgpuQueueWriteBuffer(
                this->_queue,
                buf.WgpuBuffer,
                0,
                data,
                numBytes);
        }

        template <class T>
        void WriteBuf(const AppBuffer &buf, T &data)
        {
            wgpuQueueWriteBuffer(
                this->_queue,
                buf.WgpuBuffer,
                0,
                &data,
                sizeof(T));
        }

        template <class T>
        void WriteVec(const AppBuffer &buf, std::vector<T> &vec)
        {
            wgpuQueueWriteBuffer(
                this->_queue,
                buf.WgpuBuffer,
                0,
                vec.data(),
                vec.size() * sizeof(T));
        }

        void SubmitCommandBuffer(AppCommandBuffer &buf);
        void SetWindow(std::unique_ptr<Pretop::Window::Window> win);
        void LogQueueCommands();
        WGPUTextureView GetNextSurfaceTextureView();
        WGPUTextureFormat WindowFormat;

        Application *InspectInstance();
        Application *InspectDevice();
        Application *InspectAdapter();
        Application *InspectQueue();

        std::unique_ptr<AppInstance> Instance;

    private:
        Application(StartupCallback callback);
        void CreateQueue();
        void _cleanup();

        bool _logQueueCommands;
        StartupCallback _startupCallback;
        std::unique_ptr<AppAdapter> _adapter;
        WGPUQueue _queue;
        WGPUSurface _windowSurface;
        std::unique_ptr<Pretop::Window::Window> _window;

    public:
        // This must go after the private section or else the initializer
        // list order will order the constructors incorrectly
        std::unique_ptr<AppDevice> Device;
    };
} // namespace Pretop::RHI
