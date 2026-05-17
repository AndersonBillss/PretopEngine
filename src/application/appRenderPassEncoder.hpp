#pragma once
#include <webgpu/webgpu.h>
#include <vector>
#include "appBuffer.hpp"
#include "appPipeline.hpp"

class AppRenderPassEncoder
{
public:
    AppRenderPassEncoder(WGPUCommandEncoder &encoder, WGPURenderPassDescriptor &renderPass);
    AppRenderPassEncoder &setPipeline(AppPipeline &pipeline);
    AppRenderPassEncoder &setVertexBuffers(std::vector<AppBuffer *> &vertexBuffers);
    AppRenderPassEncoder &setBindGroups(std::vector<std::unique_ptr<AppBindGroup>> &bindGroups);
    AppRenderPassEncoder &draw(size_t vertexCount);
    AppRenderPassEncoder &drawIndexed(AppBuffer &indexBuffer, size_t indexCount);
    void finish();

    WGPURenderPassEncoder wgpuRenderPass;
};