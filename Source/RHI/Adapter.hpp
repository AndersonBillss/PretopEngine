#pragma once
#include <webgpu/webgpu.h>
#include <functional>
#include <memory>
#include "Instance.hpp"

class AppAdapter
{
public:
    using RequestAdapterCallback = std::function<void(std::unique_ptr<AppAdapter>)>;
    AppAdapter(WGPUAdapter adapter);
    static void Request(AppInstance *instance, RequestAdapterCallback callback);
    WGPUAdapter WgpuAdapter;

    void Inspect();
};
