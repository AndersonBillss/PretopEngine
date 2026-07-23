#pragma once

// #include "AssetHandle.hpp"
#include "LoadGlb.hpp"
#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"

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
            std::unique_ptr<AssetLoader> assetLoader);
        ~AssetManager();

        Handle LoadModel(std::string_view path);

        Status GetState(Handle handle);
        std::unique_ptr<ParsedData> GetGlbData(Handle handle);
        std::string_view GetError(Handle handle);
        void Release(Handle handle);

    private:
        std::unique_ptr<AssetLoader> _assetLoader;
    };
} // namespace Pretop::Asset
