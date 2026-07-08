#pragma once
#include <webgpu/webgpu.h>

namespace Pretop::RHI
{
    class AppInstance
    {
    public:
        AppInstance();
        WGPUInstance WgpuInstance;

    private:
        WGPUInstance CreateInstance();
    };
} // namespace Pretop::RHI
