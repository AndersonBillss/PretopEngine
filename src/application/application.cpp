#include <iostream>
#include <unordered_map>
#include "../printStringView.hpp"
#include "application.hpp"

Application::Application() : _logQueueCommands(false),
                             _instance(AppInstance()),
                             _adapter(AppAdapter(_instance)),
                             _device(AppDevice(_instance, _adapter))
{
    createQueue();
}

void Application::setWindow(Window *win)
{
    this->_window = win;
    WGPUSurfaceConfiguration surfaceConfig = WGPU_SURFACE_CONFIGURATION_INIT;
    this->_windowSurface = win->getSurface(this->_instance.wgpuInstance);

    std::cout << "Looking for available formats" << std::endl;
    WGPUSurfaceTexture surfaceTexture = WGPU_SURFACE_TEXTURE_INIT;
    WGPUSurfaceCapabilities surfaceCapabilities = WGPU_SURFACE_CAPABILITIES_INIT;
    wgpuSurfaceGetCapabilities(this->_windowSurface, this->_adapter.wgpuAdapter, &surfaceCapabilities);

    // The first format in the list is the preffered format.
    // see https://webgpu-native.github.io/webgpu-headers/Surfaces.html#Surface-Creation
    surfaceConfig.format = surfaceCapabilities.formats[0];
    wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
    surfaceConfig.device = this->_device.wgpuDevice;
    surfaceConfig.presentMode = WGPUPresentMode_Fifo;
    surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;
    surfaceConfig.width = win->width;
    surfaceConfig.height = win->height;
    wgpuSurfaceConfigure(this->_windowSurface, &surfaceConfig);

    win->setOnExit([this]()
                   {
    wgpuSurfaceUnconfigure(this->_windowSurface);
    wgpuQueueRelease(this->_queue);
    wgpuSurfaceRelease(this->_windowSurface);
    wgpuDeviceRelease(this->_device.wgpuDevice);
    wgpuAdapterRelease(this->_adapter.wgpuAdapter);
    wgpuInstanceRelease(this->_instance.wgpuInstance); });

    win->setOnTick([this](double dt)
                   {
                       std::cout << "DeltaTime: " << dt << std::endl;
                       WGPUTextureView targetView = getNextSurfaceTextureView();
                       if (!targetView)
                           return;

                       // Create a command encoder for the draw call
                       WGPUCommandEncoderDescriptor encoderDesc = {};
                       encoderDesc.nextInChain = nullptr;
                       std::string encoderLabel = "My command encoder";
                       encoderDesc.label = {encoderLabel.c_str(), encoderLabel.size()};
                       WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(this->_device.wgpuDevice, &encoderDesc);

                       // Create the render pass that clears the screen with our color
                       WGPURenderPassDescriptor renderPassDesc = {};
                       renderPassDesc.nextInChain = nullptr;

                       // The attachment part of the render pass descriptor describes the target texture of the pass
                       WGPURenderPassColorAttachment renderPassColorAttachment = {};
                       renderPassColorAttachment.view = targetView;
                       renderPassColorAttachment.resolveTarget = nullptr;
                       renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
                       renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
                       renderPassColorAttachment.clearValue = WGPUColor{0.9, 0.1, 0.2, 1.0};
                       renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

                       renderPassDesc.colorAttachmentCount = 1;
                       renderPassDesc.colorAttachments = &renderPassColorAttachment;
                       renderPassDesc.depthStencilAttachment = nullptr;
                       renderPassDesc.timestampWrites = nullptr;

                       // Create the render pass and end it immediately (we only clear the screen but do not draw anything)
                       WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
                       wgpuRenderPassEncoderEnd(renderPass);
                       wgpuRenderPassEncoderRelease(renderPass);

                       // Finally encode and submit the render pass
                       WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
                       cmdBufferDescriptor.nextInChain = nullptr;
                       std::string cmdBufferLabel = "command buffer";
                       cmdBufferDescriptor.label = {cmdBufferLabel.c_str(), cmdBufferLabel.size()};
                       WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
                       wgpuCommandEncoderRelease(encoder);

                       std::cout << "Submitting command..." << std::endl;
                       wgpuQueueSubmit(this->_queue, 1, &command);
                       wgpuCommandBufferRelease(command);
                       std::cout << "Command submitted." << std::endl;

                       // At the end of the frame
                       wgpuTextureViewRelease(targetView);
#ifndef __EMSCRIPTEN__
                       wgpuSurfacePresent(this->_windowSurface);
#endif
                   });
}

void Application::createQueue()
{
    this->_queue = wgpuDeviceGetQueue(this->_device.wgpuDevice);

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
    this->_device.inspect();
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
