#pragma once
#include <string_view>
#include "Device.hpp"
#include "Instance.hpp"
#include "../Asset/AssetLoader.hpp"

class AppShader
{
public:
    static AppShader Pipeline(AppDevice *device, AppInstance *instance, AssetLoader *assetLoader, std::string_view src);
    WGPUShaderModule WgpuShader;
};