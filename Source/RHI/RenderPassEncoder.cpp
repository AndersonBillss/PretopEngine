#include "RenderPassEncoder.hpp"
#include <iostream>

namespace Pretop::RHI
{
    RenderPassEncoder::RenderPassEncoder(WGPUCommandEncoder &encoder, WGPURenderPassDescriptor &renderPass)
    {
        this->WgpuRenderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPass);
    }

    RenderPassEncoder &RenderPassEncoder::SetPipeline(Pipeline &pipeline)
    {
        wgpuRenderPassEncoderSetPipeline(this->WgpuRenderPass, pipeline.WgpuPipeline);
        return *this;
    }

    RenderPassEncoder &RenderPassEncoder::SetVertexBuffers(std::vector<Buffer *> &vertexBuffers)
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

    RenderPassEncoder &RenderPassEncoder::SetBindGroups(std::vector<std::unique_ptr<BindGroup>> &bindGroups)
    {
        for (size_t i = 0; i < bindGroups.size(); i++)
        {
            wgpuRenderPassEncoderSetBindGroup(this->WgpuRenderPass, i, bindGroups[i]->WgpuBindGroup, 0, nullptr);
        }
        return *this;
    }

    RenderPassEncoder &RenderPassEncoder::SetBindGroup(
        BindGroup *bindGroup, size_t groupIndex, std::vector<uint32_t> dynamicOffset)
    {
        wgpuRenderPassEncoderSetBindGroup(
            this->WgpuRenderPass,
            groupIndex,
            bindGroup->WgpuBindGroup,
            dynamicOffset.size(),
            dynamicOffset.data());
        return *this;
    }

    RenderPassEncoder &RenderPassEncoder::SetBindGroup(BindGroup *bindGroup, size_t groupIndex)
    {
        wgpuRenderPassEncoderSetBindGroup(
            this->WgpuRenderPass,
            groupIndex,
            bindGroup->WgpuBindGroup,
            0,
            nullptr);
        return *this;
    }

    RenderPassEncoder &RenderPassEncoder::Draw(size_t vertexCount)
    {
        wgpuRenderPassEncoderDraw(this->WgpuRenderPass, vertexCount, 1, 0, 0);
        return *this;
    }

    RenderPassEncoder &RenderPassEncoder::DrawIndexed(
        Buffer &indexBuffer,
        size_t indexCount,
        WGPUIndexFormat indexFormat)
    {
        wgpuRenderPassEncoderSetIndexBuffer(
            this->WgpuRenderPass, indexBuffer.WgpuBuffer, indexFormat, 0, wgpuBufferGetSize(indexBuffer.WgpuBuffer));
        wgpuRenderPassEncoderDrawIndexed(this->WgpuRenderPass, indexCount, 1, 0, 0, 0);
        return *this;
    }

    void RenderPassEncoder::Finish()
    {
        wgpuRenderPassEncoderEnd(this->WgpuRenderPass);
        wgpuRenderPassEncoderRelease(this->WgpuRenderPass);
    }
} // namespace Pretop::RHI
