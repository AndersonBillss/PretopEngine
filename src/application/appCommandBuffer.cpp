#include "appCommandBuffer.hpp"
#include <string>
#include <iostream>

const std::string cmdBufferLabel = "command buffer";

AppCommandBuffer::AppCommandBuffer(AppDevice *device)
{
    WGPUCommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    std::string encoderLabel = "My command encoder";
    encoderDesc.label = {encoderLabel.c_str(), encoderLabel.size()};
    this->wgpuEncoder = wgpuDeviceCreateCommandEncoder(device->wgpuDevice, &encoderDesc);
}

AppCommandBuffer::~AppCommandBuffer()
{
    wgpuCommandEncoderRelease(this->wgpuEncoder);
}

std::unique_ptr<AppRenderPassEncoder> AppCommandBuffer::addCommand(AppRenderPassCommand &command)
{
    return std::make_unique<AppRenderPassEncoder>(this->wgpuEncoder, command.wgpuRenderPassDescriptor);
}

void AppCommandBuffer::finish()
{
    WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = {cmdBufferLabel.c_str(), cmdBufferLabel.size()};
    this->wgpuBuffer = wgpuCommandEncoderFinish(this->wgpuEncoder, &cmdBufferDescriptor);
}
