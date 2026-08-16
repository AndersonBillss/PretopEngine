#pragma once
#include <string>
#include <vector>
#include "Device.hpp"
#include "Shader.hpp"
#include "VertexLayout.hpp"
#include "BindingLayout.hpp"

namespace Pretop::RHI
{
    class Pipeline
    {
    public:
        Pipeline(
            Device *device,
            Shader &shader,
            WGPUTextureFormat format,
            VertexLayout &vertexLayout,
            BindingLayout &pipelineLayout);
        ~Pipeline();

        WGPURenderPassDepthStencilAttachment WgpuDepthStencilAttachment;
        WGPURenderPipeline WgpuPipeline;
    };
} // namespace Pretop::RHI
