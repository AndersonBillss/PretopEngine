#include "appCommandBuffer.hpp"
#include <string>

const std::string cmdBufferLabel = "command buffer";

AppCommandBuffer::AppCommandBuffer(AppDevice &device)
{
    WGPUCommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    std::string encoderLabel = "My command encoder";
    encoderDesc.label = {encoderLabel.c_str(), encoderLabel.size()};
    this->wgpuEncoder = wgpuDeviceCreateCommandEncoder(device.wgpuDevice, &encoderDesc);
}

AppCommandBuffer::~AppCommandBuffer()
{
    wgpuCommandEncoderRelease(this->wgpuEncoder);
}

void AppCommandBuffer::addCommand(AppRenderPassCommand &command, AppPipeline &pipeline, AppVertexBuffer<float> &vertexBuffer)
{
    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(this->wgpuEncoder, &command.wgpuRenderPassDescriptor);
    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline.wgpuPipeline);

    wgpuRenderPassEncoderSetVertexBuffer(
        renderPass,
        0,
        vertexBuffer.wgpuBuffer,
        0,
        wgpuBufferGetSize(vertexBuffer.wgpuBuffer));

    wgpuRenderPassEncoderDraw(renderPass, 6, 1, 0, 0);

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);
}

void AppCommandBuffer::finish()
{
    WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = {cmdBufferLabel.c_str(), cmdBufferLabel.size()};
    this->wgpuBuffer = wgpuCommandEncoderFinish(this->wgpuEncoder, &cmdBufferDescriptor);
}
