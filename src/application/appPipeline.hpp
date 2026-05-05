#pragma once
#include <string>
#include <vector>
#include "appDevice.hpp"
#include "appShader.hpp"
#include "appVertexLayout.hpp"
#include "appBindingLayout.hpp"

class AppPipeline
{
public:
    AppPipeline(
        AppDevice device,
        AppShader shader,
        WGPUTextureFormat format,
        AppVertexLayout &vertexLayout,
        AppBindingLayout &pipelineLayout);
    ~AppPipeline();

    WGPURenderPipeline wgpuPipeline;
};
