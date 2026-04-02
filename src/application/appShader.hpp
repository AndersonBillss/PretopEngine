#pragma once
#include <string>
#include "application.hpp"

class AppShader
{
public:
    static AppShader pipeline(Application &app, std::string src);
    WGPUShaderModule wgpuShader;
};