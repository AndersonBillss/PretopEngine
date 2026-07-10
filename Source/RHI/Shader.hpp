#pragma once
#include <string_view>
#include "Device.hpp"
#include "Instance.hpp"
#include "../Asset/AssetLoader.hpp"

namespace Pretop::RHI
{
    class Shader
    {
    public:
        static Shader Pipeline(Device *device, Instance *instance, Asset::AssetLoader *assetLoader, std::string_view src);
        WGPUShaderModule WgpuShader;
    };
} // namespace Pretop::RHI
