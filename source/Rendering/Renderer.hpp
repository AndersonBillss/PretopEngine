#include "../Asset/AssetManager.hpp"
#include "../Core/GraphicsContext.hpp"
#include "../Math/Linalg/Mat4x4.hpp"

namespace Pretop::Rendering
{
    class Renderer
    {
    public:
        Renderer(Core::GraphicsContext ctx, WGPUSurface windowSurface);
        void AddObject(Asset::AssetManager::AssetReference<Asset::ParsedData> object);
        void render();
        void updateMVP(Math::Mat4x4 Model, Math::Mat4x4 View, Math::Mat4x4 Projection);

    private:
        void _initializeBindings();

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
        WGPUSurface _windowSurface;
        WGPUQueue _queue;
        WGPUBindGroup _bindGroup;
        WGPUBuffer _uniformBuffer;
        std::vector<Asset::AssetManager::AssetReference<Asset::ParsedData>> _objects;
    };
}