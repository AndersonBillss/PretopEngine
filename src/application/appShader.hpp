#pragma once
#include <string>
#include "appDevice.hpp"
#include "appInstance.hpp"

class AppShader
{
public:
    static AppShader pipeline(AppDevice &device, AppInstance &instance, std::string src);
    WGPUShaderModule wgpuShader;
};