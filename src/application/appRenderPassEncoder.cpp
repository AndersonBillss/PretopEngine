#include "appRenderPassEncoder.hpp"
#include <iostream>

AppRenderPassEncoder::AppRenderPassEncoder(WGPUCommandEncoder &encoder, WGPURenderPassDescriptor &renderPass)
{
    this->wgpuRenderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPass);
}

AppRenderPassEncoder &AppRenderPassEncoder::setPipeline(AppPipeline &pipeline)
{
    wgpuRenderPassEncoderSetPipeline(this->wgpuRenderPass, pipeline.wgpuPipeline);
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::setVertexBuffers(std::vector<AppBuffer *> &vertexBuffers)
{
    for (size_t i = 0; i < vertexBuffers.size(); i++)
    {
        auto buf = vertexBuffers[i];
        wgpuRenderPassEncoderSetVertexBuffer(
            this->wgpuRenderPass,
            i,
            buf->wgpuBuffer,
            0,
            wgpuBufferGetSize(buf->wgpuBuffer));
    }
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::setBindGroups(std::vector<std::unique_ptr<AppBindGroup>> &bindGroups)
{
    for (size_t i = 0; i < bindGroups.size(); i++)
    {
        wgpuRenderPassEncoderSetBindGroup(this->wgpuRenderPass, i, bindGroups[i]->wgpuBindGroup, 0, nullptr);
    }
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::setBindGroup(
    AppBindGroup *bindGroup, size_t groupIndex, std::vector<uint32_t> dynamicOffset)
{
    wgpuRenderPassEncoderSetBindGroup(
        this->wgpuRenderPass,
        groupIndex,
        bindGroup->wgpuBindGroup,
        dynamicOffset.size(),
        dynamicOffset.data());
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::setBindGroup(AppBindGroup *bindGroup, size_t groupIndex)
{
    wgpuRenderPassEncoderSetBindGroup(
        this->wgpuRenderPass,
        groupIndex,
        bindGroup->wgpuBindGroup,
        0,
        nullptr);
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::draw(size_t vertexCount)
{
    wgpuRenderPassEncoderDraw(this->wgpuRenderPass, vertexCount, 1, 0, 0);
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::drawIndexed(AppBuffer &indexBuffer, size_t indexCount)
{
    wgpuRenderPassEncoderSetIndexBuffer(
        this->wgpuRenderPass, indexBuffer.wgpuBuffer, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(indexBuffer.wgpuBuffer));
    wgpuRenderPassEncoderDrawIndexed(this->wgpuRenderPass, indexCount, 1, 0, 0, 0);
    return *this;
}

void AppRenderPassEncoder::finish()
{
    wgpuRenderPassEncoderEnd(this->wgpuRenderPass);
    wgpuRenderPassEncoderRelease(this->wgpuRenderPass);
}
