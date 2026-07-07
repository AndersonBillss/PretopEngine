#pragma once
#include <string>
#include <vector>
#include "Device.hpp"
#include "Shader.hpp"
#include "VertexLayout.hpp"
#include "BindingLayout.hpp"

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
