#pragma once
#include <vector>
#include <webgpu/webgpu.h>
#include <memory>
#include "Device.hpp"
#include "RenderPassCommand.hpp"
#include "Pipeline.hpp"
#include "BindGroup.hpp"
#include "Buffer.hpp"
#include "BindGroup.hpp"
#include "RenderPassEncoder.hpp"

namespace Pretop::RHI
{
    class CommandBuffer
    {
    public:
        CommandBuffer(Device *device);
        ~CommandBuffer();
        std::unique_ptr<RenderPassEncoder> AddCommand(RenderPassCommand &command);

        void Finish();
        WGPUCommandEncoder WgpuEncoder;
        WGPUCommandBuffer WgpuBuffer;
    };
} // namespace Pretop::RHI
