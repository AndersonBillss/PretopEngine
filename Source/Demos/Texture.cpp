#include "../RHI/Application.hpp"
#include "../Asset/AssetManagerFactory.hpp"
#include "../Asset/AssetLoaderFactory.hpp"
#include "../Window/WindowFactory.hpp"

struct TextureDemoData
{
    Pretop::Core::JobSystem jobs;
    std::unique_ptr<Pretop::Asset::AssetManager> Assets;

    Pretop::Asset::AssetManager::Handle ShaderHandle;
    std::unique_ptr<Pretop::RHI::Shader> Shader;
    bool ShaderLoaded = false;
};

void LoadShader(TextureDemoData *state)
{
    Pretop::Asset::AssetManager::Status shaderLoadStatus = state->Assets->GetState(state->ShaderHandle);
    if (shaderLoadStatus == Pretop::Asset::AssetManager::Status::InProgress)
    {
        return;
    }
    if (shaderLoadStatus == Pretop::Asset::AssetManager::Status::Error)
    {
        std::cout << state->Assets->GetError(state->ShaderHandle) << std::endl;
        exit(1);
    }
    state->Shader = std::move(state->Assets->GetShaderModule(state->ShaderHandle));
    std::cout << "Succesfully loaded ShaderModule" << std::endl;
    state->ShaderLoaded = true;
}

void Start(Pretop::RHI::Application &application)
{
    TextureDemoData state;
    state.Assets = std::move(
        Pretop::Asset::AssetManagerFactory::CreateAssetManager(
            std::move(Pretop::Asset::AssetLoaderFactory::CreateAssetLoader(&state.jobs)),
            application.Device.get()));

    std::unique_ptr<Pretop::Window::Window> window = Pretop::Window::WindowFactory::CreateWindow("Texture");
    application.SetWindow(std::move(window));

    state.ShaderHandle = state.Assets->LoadShaderModule("Does-not-exist");

    application.Run(
        [&state, &application](
            double dt,
            WGPUTextureView targetView)
        {
            state.jobs.PumpMainThreadCompletions();
            if (!state.ShaderLoaded)
            {
                LoadShader(&state);
            }
        });
}

namespace Pretop::Demos
{
    int Texture()
    {
        Pretop::RHI::Application::Create(Start);
        return 0;
    }
}