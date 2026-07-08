#pragma once
#include <string>
#include <vector>
#include "Device.hpp"
#include "Shader.hpp"
#include "VertexLayout.hpp"
#include "BindingLayout.hpp"

namespace Pretop::RHI
{
    class AppPipeline
    {
    public:
        AppPipeline(
            AppDevice *device,
            AppShader &shader,
            WGPUTextureFormat format,
            AppVertexLayout &vertexLayout,
            AppBindingLayout &pipelineLayout);
        ~AppPipeline();

        WGPURenderPassDepthStencilAttachment WgpuDepthStencilAttachment;
        WGPURenderPipeline WgpuPipeline;
    };
} // namespace Pretop::RHI
