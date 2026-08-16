#pragma once

#include "LoadGlb.hpp"
#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"
#include "../RHI/Application.hpp"
#include "../RHI/Shader.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace Pretop::Asset
{
    struct GPUTexture
    {
        WGPUTexture texture;
        GPUTexture(WGPUTexture texture)
        {
            this->texture = texture;
        }
        ~GPUTexture()
        {
            wgpuTextureDestroy(texture);
        }
    };

    class AssetManager
    {
        enum class AssetType
        {
            Shader,
            GLB,
        };

    public:
        using Handle = Core::Handle;
        using Status = Core::Status;

        AssetManager(
            std::unique_ptr<AssetLoader> assetLoader,
            RHI::Application *application);
        ~AssetManager();

        Handle LoadModel(std::string_view path);
        std::unique_ptr<ParsedData> GetGlbData(Handle handle);

        Handle LoadShaderModule(std::string_view path);
        std::unique_ptr<RHI::Shader> GetShaderModule(Handle handle);

        Handle LoadTexture(std::string_view path);
        std::unique_ptr<GPUTexture> GetTexture(Handle handle);

        Status GetState(Handle handle);
        std::string GetError(Handle handle);
        void Release(Handle handle);

    private:
        std::unique_ptr<AssetLoader> _assetLoader;
        RHI::Application *_application;
    };
} // namespace Pretop::Asset
