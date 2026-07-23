#pragma once
#include "AssetHandle.hpp"
#include "../Core/JobSystem.hpp"

#include <functional>
#include <string>
#include <string_view>
#include <memory>

namespace Pretop::Asset
{
    class AssetLoader
    {
    public:
        using Handle = Core::Handle;
        using Status = Core::Status;

        using RawBytesCb = void (*)(const AssetBytes &bytes, void *userData);
        using FinishCb = void (*)(std::string_view error, void *userData);

        virtual ~AssetLoader() = default;

        virtual Handle ReadFile(std::string_view path) = 0;
        virtual Handle ReadFile(std::string_view path, FinishCb finishCb, void *userData) = 0;
        virtual Handle ReadFile(std::string_view path, RawBytesCb rawBytesCb, FinishCb finishCb, void *userData) = 0;

        virtual Status GetStatus(Handle handle) const = 0;
        virtual const AssetBytes &GetBytes(Handle handle) const = 0;
        virtual void *GetRawData(Handle handle) = 0;

        virtual std::string GetError(Handle handle) const = 0;
        virtual void Release(Handle handle) = 0;
    };
} // namespace Pretop::Asset
