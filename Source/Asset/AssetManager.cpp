#include "AssetManager.hpp"

#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"

#include <exception>
#include <memory>
#include <string>
#include <utility>

namespace Pretop::Asset
{
    AssetManager::AssetManager(
        std::unique_ptr<AssetLoader> assetLoader)
        : _assetLoader(std::move(assetLoader))
    {
    }

    AssetManager::~AssetManager() = default;

    struct LoadModelData
    {
        std::unique_ptr<ParsedData> data;
    };
    AssetManager::Handle AssetManager::LoadModel(std::string_view path)
    {
        LoadModelData *loadModelData = new LoadModelData;
        loadModelData->data = nullptr;
        return _assetLoader->ReadFile(
            path,
            [](const AssetBytes &bytes, void *userData)
            {
                LoadModelData *data = reinterpret_cast<LoadModelData *>(userData);
                data->data = std::make_unique<ParsedData>(LoadGlb(bytes));
            },
            [](AssetLoader &loader, AssetManager::Handle handle) {
                loader.Release(handle);
            },
            loadModelData);
    }

    AssetManager::Status AssetManager::GetState(Handle handle)
    {
        return _assetLoader->GetStatus(handle);
    }

    std::unique_ptr<ParsedData> AssetManager::GetGlbData(Handle handle)
    {
        LoadModelData *loadModelData = reinterpret_cast<LoadModelData *>(_assetLoader->GetRawData(handle));
        return std::move(loadModelData->data);
    }

    std::string_view AssetManager::GetError(Handle handle)
    {
        return _assetLoader->GetError(handle);
    }
    void AssetManager::Release(Handle handle)
    {
        return _assetLoader->Release(handle);
    }
} // namespace Pretop::Asset
