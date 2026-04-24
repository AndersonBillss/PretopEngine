#include <iostream>
#include <unordered_map>
#include "../printStringView.hpp"
#include "application.hpp"

Application::Application() : _logQueueCommands(false),
                             instance(AppInstance()),
                             _adapter(AppAdapter(instance)),
                             device(AppDevice(instance, _adapter))
{
    _createQueue();
}

void Application::run(TickCallback cb)
{
    _window->setOnTick([this, cb](double dt)
                       {
                           WGPUTextureView targetView = getNextSurfaceTextureView();
                           if (!targetView)
                               return;

                           cb(dt, targetView);

                           wgpuTextureViewRelease(targetView);
#ifndef __EMSCRIPTEN__
                           wgpuSurfacePresent(this->_windowSurface);
#endif
                       });
    _window->run();
}

void Application::writeVertices(const std::initializer_list<AppVertexBuffer<float> *> &bufs)
{
    for (const auto buf : bufs)
    {
        wgpuQueueWriteBuffer(
            this->_queue,
            buf->wgpuBuffer,
            0,
            buf->data(),
            buf->numBytes());
    }
}

void Application::writeIndex(const AppIndexBuffer<uint16_t> &buf)
{
    wgpuQueueWriteBuffer(
        this->_queue,
        buf.wgpuBuffer,
        0,
        buf.data(),
        buf.numBytes());
}
void Application::writeIndex(const AppIndexBuffer<uint32_t> &buf)
{
    wgpuQueueWriteBuffer(
        this->_queue,
        buf.wgpuBuffer,
        0,
        buf.data(),
        buf.numBytes());
}

void Application::submitCommandBuffer(AppCommandBuffer &buf)
{
    wgpuQueueSubmit(this->_queue, 1, &buf.wgpuBuffer);
}

void Application::setWindow(std::unique_ptr<Window> win)
{
    this->_window = std::move(win);
    WGPUSurfaceConfiguration surfaceConfig = WGPU_SURFACE_CONFIGURATION_INIT;
    this->_windowSurface = _window->getSurface(this->instance.wgpuInstance);

    WGPUSurfaceTexture surfaceTexture = WGPU_SURFACE_TEXTURE_INIT;
    WGPUSurfaceCapabilities surfaceCapabilities = WGPU_SURFACE_CAPABILITIES_INIT;
    wgpuSurfaceGetCapabilities(this->_windowSurface, this->_adapter.wgpuAdapter, &surfaceCapabilities);

    // The first format in the list is the preffered format.
    // see https://webgpu-native.github.io/webgpu-headers/Surfaces.html#Surface-Creation
    surfaceConfig.format = surfaceCapabilities.formats[0];
    this->windowFormat = surfaceCapabilities.formats[0];
    wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
    surfaceConfig.device = this->device.wgpuDevice;
    surfaceConfig.presentMode = WGPUPresentMode_Fifo;
    surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;
    surfaceConfig.width = _window->width;
    surfaceConfig.height = _window->height;
    wgpuSurfaceConfigure(this->_windowSurface, &surfaceConfig);

    _window->setOnExit([this]()
                       {
    wgpuSurfaceUnconfigure(this->_windowSurface);
    wgpuQueueRelease(this->_queue);
    wgpuSurfaceRelease(this->_windowSurface);
    wgpuDeviceRelease(this->device.wgpuDevice);
    wgpuAdapterRelease(this->_adapter.wgpuAdapter);
    wgpuInstanceRelease(this->instance.wgpuInstance); });
}

void Application::_createQueue()
{
    this->_queue = wgpuDeviceGetQueue(this->device.wgpuDevice);

    WGPUQueueWorkDoneCallback onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, WGPUStringView message, void *data, void *)
    {
        auto self = static_cast<Application *>(data);
        if (self->_logQueueCommands)
        {
            std::unordered_map<WGPUQueueWorkDoneStatus, std::string> statusToString = {
                {WGPUQueueWorkDoneStatus_Success, "WGPUQueueWorkDoneStatus_Success"},
                {WGPUQueueWorkDoneStatus_CallbackCancelled, "WGPUQueueWorkDoneStatus_CallbackCancelled"},
                {WGPUQueueWorkDoneStatus_Error, "WGPUQueueWorkDoneStatus_Error"},
            };
            std::string stringifiedStatus = "Unknown";
            auto it = statusToString.find(status);
            if (it != statusToString.end())
            {
                stringifiedStatus = it->second;
            }
            std::cout << "Queued work finished with status: " << stringifiedStatus << std::endl;
            std::cout << message << std::endl;
        }
    };

    WGPUQueueWorkDoneCallbackInfo workQueueWorkDoneCb = {
        /* nextInChain */ nullptr,
        /* mode */ WGPUCallbackMode_AllowSpontaneous,
        /* callback */ onQueueWorkDone,
        /* userdata1 */ this,
        /* userdata2 */ nullptr};
    wgpuQueueOnSubmittedWorkDone(this->_queue, workQueueWorkDoneCb);
}

void Application::logQueueCommands()
{
    this->_logQueueCommands = true;
}

Application *Application::inspectInstance()
{
    return this;
}

Application *Application::inspectDevice()
{
    this->device.inspect();
    return this;
}

Application *Application::inspectAdapter()
{
    this->_adapter.inspect();
    return this;
}

Application *Application::inspectQueue()
{
    return this;
}

WGPUTextureView Application::getNextSurfaceTextureView()
{
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(this->_windowSurface, &surfaceTexture);
    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
    {
        return nullptr;
    }

    WGPUTextureViewDescriptor viewDescriptor;
    viewDescriptor.nextInChain = nullptr;
    std::string label = "Surface texture view";
    viewDescriptor.label = WGPUStringView{label.c_str(), label.size()};
    viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.usage = WGPUTextureUsage_RenderAttachment;
    viewDescriptor.aspect = WGPUTextureAspect_All;
    WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

    wgpuTextureRelease(surfaceTexture.texture);
    return targetView;
}
