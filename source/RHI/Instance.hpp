#pragma once
#include <webgpu/webgpu.h>

namespace Pretop::RHI
{
    class Instance
    {
    public:
        Instance();
        WGPUInstance WgpuInstance;

    private:
        WGPUInstance CreateInstance();
    };
} // namespace Pretop::RHI
