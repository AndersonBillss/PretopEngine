#pragma once
#include <string_view>
#include <memory>
#include "Device.hpp"
#include "Instance.hpp"
#include "../Asset/AssetLoader.hpp"
#include "../Core/GraphicsContext.hpp"

namespace Pretop::RHI
{
    class Shader
    {
    public:
        static Shader Pipeline(Core::GraphicsContext context, const void *src, size_t srcSize);
        WGPUShaderModule WgpuShader;
    };
} // namespace Pretop::RHI
