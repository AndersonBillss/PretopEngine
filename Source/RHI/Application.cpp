#include <iostream>
#include <unordered_map>
#include "../PrintStringView.hpp"
#include "Application.hpp"

namespace Pretop::RHI
{
    Application::Application(StartupCallback callback)
    {
        this->_startupCallback = callback;
        this->_logQueueCommands = false;
        this->Instance = nullptr;
        this->_adapter = nullptr;
        this->Device = nullptr;
    }

    void Application::Create(StartupCallback callback)
    {
        Application *application = new Application(callback);
        application->Instance = std::make_unique<Instance>();
        Adapter::Request(application->Instance.get(), [application](std::unique_ptr<Adapter> adapter)
                            {
            application->_adapter = std::move(adapter);
            Device::Request(application->Instance.get(), application->_adapter.get(), [application](std::unique_ptr<Device> device) {
                application->Device = std::move(device);
                application->CreateQueue();
                application->_startupCallback(*application);
                delete application;
            }); });
    }

    void Application::Run(TickCallback callback)
    {
        _window->SetOnTick([this, callback](double dt)
                           {
                               WGPUTextureView displayView = GetNextSurfaceTextureView();
                               if (!displayView)
                                   return;

                               callback(dt, displayView);
                               wgpuTextureViewRelease(displayView);
#ifndef __EMSCRIPTEN__
                               wgpuSurfacePresent(this->_windowSurface);
#endif
                           });
        _window->Run();
    }

    void Application::SubmitCommandBuffer(CommandBuffer &buf)
    {
        wgpuQueueSubmit(this->_queue, 1, &buf.WgpuBuffer);
    }

    void Application::SetWindow(std::unique_ptr<Pretop::Window::Window> win)
    {
        this->_window = std::move(win);
        WGPUSurfaceConfiguration surfaceConfig = WGPU_SURFACE_CONFIGURATION_INIT;
        this->_windowSurface = _window->GetSurface(this->Instance->WgpuInstance);

        WGPUSurfaceTexture surfaceTexture = WGPU_SURFACE_TEXTURE_INIT;
        WGPUSurfaceCapabilities surfaceCapabilities = WGPU_SURFACE_CAPABILITIES_INIT;
        wgpuSurfaceGetCapabilities(this->_windowSurface, this->_adapter->WgpuAdapter, &surfaceCapabilities);

        // The first format in the list is the preffered format.
        // see https://webgpu-native.github.io/webgpu-headers/Surfaces.html#Surface-Creation
        surfaceConfig.format = surfaceCapabilities.formats[0];
        this->WindowFormat = surfaceCapabilities.formats[0];
        wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);
        surfaceConfig.viewFormatCount = 0;
        surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
        surfaceConfig.device = this->Device->WgpuDevice;
        surfaceConfig.presentMode = WGPUPresentMode_Fifo;
        surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;
        surfaceConfig.width = _window->Width;
        surfaceConfig.height = _window->Height;
        wgpuSurfaceConfigure(this->_windowSurface, &surfaceConfig);

        _window->SetOnExit([this]()
                           {
            wgpuSurfaceUnconfigure(this->_windowSurface);
            wgpuQueueRelease(this->_queue);
            wgpuSurfaceRelease(this->_windowSurface);
            wgpuDeviceRelease(this->Device->WgpuDevice);
            wgpuAdapterRelease(this->_adapter->WgpuAdapter);
            wgpuInstanceRelease(this->Instance->WgpuInstance); });
    }

    void Application::CreateQueue()
    {
        this->_queue = wgpuDeviceGetQueue(this->Device->WgpuDevice);

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

    void Application::LogQueueCommands()
    {
        this->_logQueueCommands = true;
    }

    Application *Application::InspectInstance()
    {
        return this;
    }

    Application *Application::InspectDevice()
    {
        this->Device->Inspect();
        return this;
    }

    Application *Application::InspectAdapter()
    {
        this->_adapter->Inspect();
        return this;
    }

    Application *Application::InspectQueue()
    {
        return this;
    }

    WGPUTextureView Application::GetNextSurfaceTextureView()
    {
        WGPUSurfaceTexture surfaceTexture = WGPU_SURFACE_TEXTURE_INIT;
        wgpuSurfaceGetCurrentTexture(this->_windowSurface, &surfaceTexture);
        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
            surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
        {
            return nullptr;
        }

        WGPUTextureViewDescriptor viewDescriptor = WGPU_TEXTURE_VIEW_DESCRIPTOR_INIT;
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
} // namespace Pretop::RHI
