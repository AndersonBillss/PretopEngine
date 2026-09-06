#pragma once

#include "LoadGlb.hpp"
#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"
#include "../Core/GraphicsContext.hpp"
#include "../Core/PagedVector.hpp"
#include "../Asset/GPUTexture.hpp"
#include "../RHI/Shader.hpp"

#include <memory>
#include <string>
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
            Core::GraphicsContext graphicsContext);
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
        Core::GraphicsContext _graphicsContext;
    };
} // namespace Pretop::Asset
