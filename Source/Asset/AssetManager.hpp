#pragma once

#include "LoadGlb.hpp"
#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"
#include "../RHI/Device.hpp"
#include "../RHI/Shader.hpp"

#include <memory>
#include <string_view>

namespace Pretop::Asset
{
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
            RHI::Device *device);
        ~AssetManager();

        Handle LoadModel(std::string_view path);
        std::unique_ptr<ParsedData> GetGlbData(Handle handle);

        Handle LoadShaderModule(std::string_view path);
        std::unique_ptr<RHI::Shader> GetShaderModule(Handle handle);

        Status GetState(Handle handle);
        std::string_view GetError(Handle handle);
        void Release(Handle handle);

    private:
        std::unique_ptr<AssetLoader> _assetLoader;
        RHI::Device *_device;
    };
} // namespace Pretop::Asset
