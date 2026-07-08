#include "CommandBuffer.hpp"
#include <string>
#include <iostream>

namespace Pretop::RHI
{
    const std::string cmdBufferLabel = "command buffer";

    AppCommandBuffer::AppCommandBuffer(AppDevice *device)
    {
        WGPUCommandEncoderDescriptor encoderDesc = {};
        encoderDesc.nextInChain = nullptr;
        std::string encoderLabel = "My command encoder";
        encoderDesc.label = {encoderLabel.c_str(), encoderLabel.size()};
        this->WgpuEncoder = wgpuDeviceCreateCommandEncoder(device->WgpuDevice, &encoderDesc);
    }

    AppCommandBuffer::~AppCommandBuffer()
    {
        wgpuCommandEncoderRelease(this->WgpuEncoder);
    }

    std::unique_ptr<AppRenderPassEncoder> AppCommandBuffer::AddCommand(AppRenderPassCommand &command)
    {
        return std::make_unique<AppRenderPassEncoder>(this->WgpuEncoder, command.WgpuRenderPassDescriptor);
    }

    void AppCommandBuffer::Finish()
    {
        WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
        cmdBufferDescriptor.nextInChain = nullptr;
        cmdBufferDescriptor.label = {cmdBufferLabel.c_str(), cmdBufferLabel.size()};
        this->WgpuBuffer = wgpuCommandEncoderFinish(this->WgpuEncoder, &cmdBufferDescriptor);
    }
} // namespace Pretop::RHI
