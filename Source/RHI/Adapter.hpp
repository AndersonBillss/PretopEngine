#pragma once
#include <webgpu/webgpu.h>
#include <functional>
#include <memory>
#include "Instance.hpp"

namespace Pretop::RHI
{
    class Adapter
    {
    public:
        using RequestAdapterCallback = std::function<void(std::unique_ptr<Adapter>)>;
        Adapter(WGPUAdapter adapter);
        static void Request(Instance *instance, RequestAdapterCallback callback);
        WGPUAdapter WgpuAdapter;

        void Inspect();
    };
} // namespace Pretop::RHI
