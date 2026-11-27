#include "../Asset/AssetManager.hpp"
#include "../Core/GraphicsContext.hpp"
#include "../Math/Linalg/Mat4x4.hpp"

namespace Pretop::Rendering
{
    class Renderer
    {
    public:
        struct GpuMesh
        {
            WGPUBuffer vertexBuffer;
            WGPUBuffer indexBuffer;
            WGPUIndexFormat indexFormat;
        };

        Renderer(Pretop::Asset::AssetManager *assetManager, Core::GraphicsContext ctx, WGPUTextureFormat windowFormat);
        void AddObject(GpuMesh object);
        void Render(WGPUTextureView windowView);
        void UpdateMVP(Math::Mat4x4 Model, Math::Mat4x4 View, Math::Mat4x4 Projection);
        bool PollReady();

    private:
        void _afterShadersLoadedStep(WGPUShaderModule shaderModule);

        struct MyUniforms
        {
            Math::Mat4x4 ProjectionMatrix;
            Math::Mat4x4 ViewMatrix;
            Math::Mat4x4 ModelMatrix;
            float Color;
            float Time;
            float Pad[2];
        };

        MyUniforms _uniforms;
        Core::GraphicsContext _ctx;
        Asset::AssetManager *_assetManager;
        Asset::AssetManager::AssetReference<RHI::Shader> _shaderRef;
        WGPUTextureView _windowView;
        WGPURenderPassDepthStencilAttachment _depthStencilAttachment;
        WGPUBindGroup _bindGroup;
        WGPUBuffer _uniformBuffer;
        WGPURenderPipeline _pipeline;
        WGPUTextureFormat _windowFormat;
        std::vector<GpuMesh> _objects;
        bool _shaderLoaded;
    };
}