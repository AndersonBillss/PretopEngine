#pragma once
#include <string>
#include "application.hpp"
#include "appShader.hpp"

class AppPipeline
{
public:
    AppPipeline(Application &app, AppShader shader);
    ~AppPipeline();

    WGPURenderPipeline wgpuPipeline;
};
