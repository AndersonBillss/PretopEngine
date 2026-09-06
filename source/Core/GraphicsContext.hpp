#pragma once
#include <webgpu/webgpu.h>

namespace Pretop::Core
{
    struct GraphicsContext
    {
        WGPUDevice Device;
        WGPUQueue Queue;
    };
}
