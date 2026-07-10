#pragma once
#include <webgpu/webgpu.h>
#include <memory>
#include <vector>
#include "Buffer.hpp"
#include "Pipeline.hpp"

namespace Pretop::RHI
{
    class RenderPassEncoder
    {
    public:
        RenderPassEncoder(WGPUCommandEncoder &encoder, WGPURenderPassDescriptor &renderPass);
        RenderPassEncoder &SetPipeline(Pipeline &pipeline);
        RenderPassEncoder &SetVertexBuffers(std::vector<Buffer *> &vertexBuffers);
        RenderPassEncoder &SetBindGroups(std::vector<std::unique_ptr<BindGroup>> &bindGroups);
        RenderPassEncoder &SetBindGroup(BindGroup *bindGroup, size_t groupIndex, std::vector<uint32_t> dynamicOffset);
        RenderPassEncoder &SetBindGroup(BindGroup *bindGroup, size_t groupIndex);
        RenderPassEncoder &Draw(size_t vertexCount);
        RenderPassEncoder &DrawIndexed(Buffer &indexBuffer,
                                          size_t indexCount,
                                          WGPUIndexFormat indexFormat);
        void Finish();

        WGPURenderPassEncoder WgpuRenderPass;
    };
} // namespace Pretop::RHI
