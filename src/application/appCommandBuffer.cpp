#include "appCommandBuffer.hpp"
#include <string>
#include <iostream>

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

void AppCommandBuffer::addCommand(
    AppRenderPassCommand &command,
    AppPipeline &pipeline,
    std::vector<AppBuffer *> &vertexBuffers,
    AppVertexLayout &layout,
    std::vector<std::unique_ptr<AppBindGroup>> &bindGroups)
{
    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(this->wgpuEncoder, &command.wgpuRenderPassDescriptor);
    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline.wgpuPipeline);

    for (size_t i = 0; i < vertexBuffers.size(); i++)
    {
        auto buf = vertexBuffers[i];
        wgpuRenderPassEncoderSetVertexBuffer(
            renderPass,
            i,
            buf->wgpuBuffer,
            0,
            wgpuBufferGetSize(buf->wgpuBuffer));
    }
    for (size_t i = 0; i < bindGroups.size(); i++)
    {
        wgpuRenderPassEncoderSetBindGroup(renderPass, i, bindGroups[i]->wgpuBindGroup, 0, nullptr);
    }
    size_t numRows = vertexBuffers[0]->numBytes() / layout.bufferLayouts[0].arrayStride;
    wgpuRenderPassEncoderDraw(renderPass, numRows, 1, 0, 0);

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);
}

void AppCommandBuffer::addCommand(
    AppRenderPassCommand &command,
    AppPipeline &pipeline,
    std::vector<AppBuffer *> &vertexBuffers,
    AppVertexLayout &layout,
    AppBuffer &indexBuffer,
    size_t indexBufferSize,
    std::vector<std::unique_ptr<AppBindGroup>> &bindGroups)
{
    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(this->wgpuEncoder, &command.wgpuRenderPassDescriptor);
    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline.wgpuPipeline);

    for (size_t i = 0; i < vertexBuffers.size(); i++)
    {
        auto buf = vertexBuffers[i];
        wgpuRenderPassEncoderSetVertexBuffer(
            renderPass,
            i,
            buf->wgpuBuffer,
            0,
            wgpuBufferGetSize(buf->wgpuBuffer));
    }
    wgpuRenderPassEncoderSetIndexBuffer(
        renderPass, indexBuffer.wgpuBuffer, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(indexBuffer.wgpuBuffer));

    for (size_t i = 0; i < bindGroups.size(); i++)
    {
        wgpuRenderPassEncoderSetBindGroup(renderPass, i, bindGroups[i]->wgpuBindGroup, 0, nullptr);
    }
    wgpuRenderPassEncoderDrawIndexed(renderPass, indexBuffer.numBytes() / indexBufferSize, 1, 0, 0, 0);

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);
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
