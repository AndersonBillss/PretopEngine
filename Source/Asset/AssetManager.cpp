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
        std::unique_ptr<AssetLoader> assetLoader,
        RHI::Device *device)
        : _assetLoader(std::move(assetLoader)), _device(device)
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
            [](const AssetLoader::AssetBytes &bytes, void *userData)
            {
                LoadModelData *data = reinterpret_cast<LoadModelData *>(userData);
                data->data = std::make_unique<ParsedData>(LoadGlb(bytes));
            },
            [](AssetLoader &loader, AssetManager::Handle handle) {},
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

    struct LoadShaderModuleData
    {
        std::unique_ptr<RHI::Shader> data;
        RHI::Device *device;
    };
    AssetManager::Handle AssetManager::LoadShaderModule(std::string_view path)
    {
        LoadShaderModuleData *loadShaderModuleData = new LoadShaderModuleData;
        loadShaderModuleData->data = nullptr;
        loadShaderModuleData->device = this->_device;
        return _assetLoader->ReadFile(
            path,
            [](AssetLoader &loader, AssetManager::Handle handle)
            {
                AssetLoader::AssetBytes bytes = loader.GetBytes(handle);
                LoadShaderModuleData *data = reinterpret_cast<LoadShaderModuleData *>(loader.GetRawData(handle));
                data->data = std::make_unique<RHI::Shader>(RHI::Shader::Pipeline(data->device, bytes.data(), bytes.size()));
            },
            loadShaderModuleData);
    }

    std::unique_ptr<RHI::Shader> AssetManager::GetShaderModule(Handle handle)
    {
        LoadShaderModuleData *loadShaderModuleData = reinterpret_cast<LoadShaderModuleData *>(_assetLoader->GetRawData(handle));
        return std::move(loadShaderModuleData->data);
    }

    std::string_view AssetManager::GetError(Handle handle)
    {
        return _assetLoader->GetError(handle);
    }
    void AssetManager::Release(Handle handle)
    {
        delete static_cast<LoadModelData *>(_assetLoader->GetRawData(handle));
        return _assetLoader->Release(handle);
    }
} // namespace Pretop::Asset
