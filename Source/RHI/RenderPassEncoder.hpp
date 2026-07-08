#pragma once
#include <webgpu/webgpu.h>
#include <memory>
#include <vector>
#include "Buffer.hpp"
#include "Pipeline.hpp"

namespace Pretop::RHI
{
    class AppRenderPassEncoder
    {
    public:
        AppRenderPassEncoder(WGPUCommandEncoder &encoder, WGPURenderPassDescriptor &renderPass);
        AppRenderPassEncoder &SetPipeline(AppPipeline &pipeline);
        AppRenderPassEncoder &SetVertexBuffers(std::vector<AppBuffer *> &vertexBuffers);
        AppRenderPassEncoder &SetBindGroups(std::vector<std::unique_ptr<AppBindGroup>> &bindGroups);
        AppRenderPassEncoder &SetBindGroup(AppBindGroup *bindGroup, size_t groupIndex, std::vector<uint32_t> dynamicOffset);
        AppRenderPassEncoder &SetBindGroup(AppBindGroup *bindGroup, size_t groupIndex);
        AppRenderPassEncoder &Draw(size_t vertexCount);
        AppRenderPassEncoder &DrawIndexed(AppBuffer &indexBuffer,
                                          size_t indexCount,
                                          WGPUIndexFormat indexFormat);
        void Finish();

        WGPURenderPassEncoder WgpuRenderPass;
    };
} // namespace Pretop::RHI
