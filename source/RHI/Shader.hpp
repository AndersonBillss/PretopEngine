#pragma once
#include <string_view>
#include "Device.hpp"
#include "Instance.hpp"
#include "../Asset/AssetLoader.hpp"
#include <memory>

namespace Pretop::RHI
{
    class Shader
    {
    public:
        static Shader Pipeline(Device *device, const void *src, size_t srcSize);
        WGPUShaderModule WgpuShader;
    };
} // namespace Pretop::RHI
