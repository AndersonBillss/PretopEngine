#pragma once
#include <webgpu/webgpu.h>

// Default WebGPU limits
constexpr WGPULimits defaultLimits = {
    nullptr,                 // nextInChain
    8192,                    // maxTextureDimension1D
    8192,                    // maxTextureDimension2D
    2048,                    // maxTextureDimension3D
    256,                     // maxTextureArrayLayers
    4,                       // maxBindGroups
    24,                      // maxBindGroupsPlusVertexBuffers
    1000,                    // maxBindingsPerBindGroup
    8,                       // maxDynamicUniformBuffersPerPipelineLayout
    4,                       // maxDynamicStorageBuffersPerPipelineLayout
    16,                      // maxSampledTexturesPerShaderStage
    16,                      // maxSamplersPerShaderStage
    8,                       // maxStorageBuffersPerShaderStage
    4,                       // maxStorageTexturesPerShaderStage
    12,                      // maxUniformBuffersPerShaderStage
    65536,                   // maxUniformBufferBindingSize
    134217728,               // maxStorageBufferBindingSize
    256,                     // minUniformBufferOffsetAlignment
    256,                     // minStorageBufferOffsetAlignment
    8,                       // maxVertexBuffers
    268435456,               // maxBufferSize
    16,                      // maxVertexAttributes
    2048,                    // maxVertexBufferArrayStride
    16,                      // maxInterStageShaderVariables
    8,                       // maxColorAttachments
    32,                      // maxColorAttachmentBytesPerSample
    16384,                   // maxComputeWorkgroupStorageSize
    256,                     // maxComputeInvocationsPerWorkgroup
    256,                     // maxComputeWorkgroupSizeX
    256,                     // maxComputeWorkgroupSizeY
    64,                      // maxComputeWorkgroupSizeZ
    65535,                   // maxComputeWorkgroupsPerDimension
    WGPU_LIMIT_U32_UNDEFINED // maxImmediateSize
};

// Compatibility mode defaults
constexpr WGPULimits compatibilityLimits = {
    nullptr,                 // nextInChain
    4096,                    // maxTextureDimension1D
    4096,                    // maxTextureDimension2D
    2048,                    // maxTextureDimension3D
    256,                     // maxTextureArrayLayers
    4,                       // maxBindGroups
    24,                      // maxBindGroupsPlusVertexBuffers
    1000,                    // maxBindingsPerBindGroup
    8,                       // maxDynamicUniformBuffersPerPipelineLayout
    4,                       // maxDynamicStorageBuffersPerPipelineLayout
    16,                      // maxSampledTexturesPerShaderStage
    16,                      // maxSamplersPerShaderStage
    8,                       // maxStorageBuffersPerShaderStage
    4,                       // maxStorageTexturesPerShaderStage
    12,                      // maxUniformBuffersPerShaderStage
    16384,                   // maxUniformBufferBindingSize
    134217728,               // maxStorageBufferBindingSize
    256,                     // minUniformBufferOffsetAlignment
    256,                     // minStorageBufferOffsetAlignment
    8,                       // maxVertexBuffers
    268435456,               // maxBufferSize
    16,                      // maxVertexAttributes
    2048,                    // maxVertexBufferArrayStride
    15,                      // maxInterStageShaderVariables
    4,                       // maxColorAttachments
    32,                      // maxColorAttachmentBytesPerSample
    16384,                   // maxComputeWorkgroupStorageSize
    128,                     // maxComputeInvocationsPerWorkgroup
    128,                     // maxComputeWorkgroupSizeX
    128,                     // maxComputeWorkgroupSizeY
    64,                      // maxComputeWorkgroupSizeZ
    65535,                   // maxComputeWorkgroupsPerDimension
    WGPU_LIMIT_U32_UNDEFINED // maxImmediateSize
};