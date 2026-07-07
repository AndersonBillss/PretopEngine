#include "RenderPassEncoder.hpp"
#include <iostream>

AppRenderPassEncoder::AppRenderPassEncoder(WGPUCommandEncoder &encoder, WGPURenderPassDescriptor &renderPass)
{
    this->WgpuRenderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPass);
}

AppRenderPassEncoder &AppRenderPassEncoder::SetPipeline(AppPipeline &pipeline)
{
    wgpuRenderPassEncoderSetPipeline(this->WgpuRenderPass, pipeline.WgpuPipeline);
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::SetVertexBuffers(std::vector<AppBuffer *> &vertexBuffers)
{
    for (size_t i = 0; i < vertexBuffers.size(); i++)
    {
        auto buf = vertexBuffers[i];
        wgpuRenderPassEncoderSetVertexBuffer(
            this->WgpuRenderPass,
            i,
            buf->WgpuBuffer,
            0,
            wgpuBufferGetSize(buf->WgpuBuffer));
    }
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::SetBindGroups(std::vector<std::unique_ptr<AppBindGroup>> &bindGroups)
{
    for (size_t i = 0; i < bindGroups.size(); i++)
    {
        wgpuRenderPassEncoderSetBindGroup(this->WgpuRenderPass, i, bindGroups[i]->WgpuBindGroup, 0, nullptr);
    }
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::SetBindGroup(
    AppBindGroup *bindGroup, size_t groupIndex, std::vector<uint32_t> dynamicOffset)
{
    wgpuRenderPassEncoderSetBindGroup(
        this->WgpuRenderPass,
        groupIndex,
        bindGroup->WgpuBindGroup,
        dynamicOffset.size(),
        dynamicOffset.data());
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::SetBindGroup(AppBindGroup *bindGroup, size_t groupIndex)
{
    wgpuRenderPassEncoderSetBindGroup(
        this->WgpuRenderPass,
        groupIndex,
        bindGroup->WgpuBindGroup,
        0,
        nullptr);
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::Draw(size_t vertexCount)
{
    wgpuRenderPassEncoderDraw(this->WgpuRenderPass, vertexCount, 1, 0, 0);
    return *this;
}

AppRenderPassEncoder &AppRenderPassEncoder::DrawIndexed(
    AppBuffer &indexBuffer,
    size_t indexCount,
    WGPUIndexFormat indexFormat)
{
    wgpuRenderPassEncoderSetIndexBuffer(
        this->WgpuRenderPass, indexBuffer.WgpuBuffer, indexFormat, 0, wgpuBufferGetSize(indexBuffer.WgpuBuffer));
    wgpuRenderPassEncoderDrawIndexed(this->WgpuRenderPass, indexCount, 1, 0, 0, 0);
    return *this;
}

void AppRenderPassEncoder::Finish()
{
    wgpuRenderPassEncoderEnd(this->WgpuRenderPass);
    wgpuRenderPassEncoderRelease(this->WgpuRenderPass);
}
