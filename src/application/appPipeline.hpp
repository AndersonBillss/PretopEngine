#pragma once
#include <string>
#include <vector>
#include "appDevice.hpp"
#include "appShader.hpp"
#include "appVertexBufferLayout.hpp"

class AppPipeline
{
public:
    AppPipeline(
        AppDevice device,
        AppShader shader,
        WGPUTextureFormat format,
        std::vector<AppVertexBufferLayout> vertexLayout);
    ~AppPipeline();

    WGPURenderPipeline wgpuPipeline;
};
