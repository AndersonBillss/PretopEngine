#pragma once
#include <string_view>
#include "Device.hpp"
#include "Instance.hpp"
#include "../Asset/AssetLoader.hpp"

namespace Pretop::RHI
{
    class AppShader
    {
    public:
        static AppShader Pipeline(AppDevice *device, AppInstance *instance, Asset::AssetLoader *assetLoader, std::string_view src);
        WGPUShaderModule WgpuShader;
    };
} // namespace Pretop::RHI
