#pragma once
#include <string_view>
#include "appDevice.hpp"
#include "appInstance.hpp"
#include "../asset/assetLoader.hpp"

class AppShader
{
public:
    static AppShader pipeline(AppDevice &device, AppInstance &instance, AssetLoader *assetLoader, std::string_view src);
    WGPUShaderModule wgpuShader;
};