#pragma once
#include <string>
#include <vector>
#include "appDevice.hpp"
#include "appShader.hpp"
#include "appVertexLayout.hpp"

class AppPipeline
{
public:
    AppPipeline(
        AppDevice device,
        AppShader shader,
        WGPUTextureFormat format,
        AppVertexLayout &vertexLayout,
        WGPUPipelineLayout &pipelineLayout);
    ~AppPipeline();

    WGPURenderPipeline wgpuPipeline;
};
