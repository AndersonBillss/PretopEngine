#include <catch2/catch_test_macros.hpp>

#include "../../Source/Asset/NATIVE_AssetLoader.hpp"
#include "../../Source/Asset/AssetManager.hpp"
#include "../../Source/Core/JobSystem.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace Pretop::Asset;
using namespace Pretop::Core;

void WaitUntilDone(AssetManager &assetManager, Handle handle)
{
    while (assetManager.GetState(handle) == AssetManager::Status::InProgress)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

namespace
{
    std::string AssetPath(const char *filename)
    {
        const std::filesystem::path projectRoot =
            std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
        return (projectRoot / "assets" / "models" / filename).string();
    }

    AssetBytes ReadBytes(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error("Failed to open test asset: " + path);
        }

        const std::streampos end = file.tellg();
        AssetBytes bytes(static_cast<std::size_t>(end));
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
        return bytes;
    }
}

TEST_CASE("LoadGlb parses model bytes", "[Asset][LoadGlb]")
{
    const AssetBytes bytes = ReadBytes(AssetPath("DiffuseTransmissionPlant.glb"));
    const ParsedData model = LoadGlb(bytes);

    REQUIRE_FALSE(model.Vertices.empty());
    std::visit([](const auto &indices)
               { REQUIRE_FALSE(indices.empty()); },
               model.Indices);
}

TEST_CASE("AssetManager loads models concurrently", "[Asset][AssetManager]")
{
    JobSystem jobSystem;
    AssetManager assetManager(std::make_unique<NativeAssetLoader>(&jobSystem));

    Handle plantHandle = assetManager.LoadModel(
        AssetPath("DiffuseTransmissionPlant.glb"));
    Handle mammothHandle = assetManager.LoadModel(
        AssetPath("woolly-mammoth-100k-4096_std.glb"));

    WaitUntilDone(assetManager, plantHandle);
    WaitUntilDone(assetManager, mammothHandle);

    const auto plantResult = assetManager.GetGlbData(plantHandle);
    const auto mammothResult = assetManager.GetGlbData(mammothHandle);

    REQUIRE(plantResult);
    REQUIRE(mammothResult);
    REQUIRE_FALSE(plantResult->Vertices.empty());
    REQUIRE_FALSE(mammothResult->Vertices.empty());
}

TEST_CASE("AssetManager reports byte loading errors", "[Asset][AssetManager]")
{
    JobSystem jobSystem;
    AssetManager assetManager(std::make_unique<NativeAssetLoader>(&jobSystem));

    auto handle = assetManager.LoadModel(AssetPath("missing.glb"));
    WaitUntilDone(assetManager, handle);

    const auto result = assetManager.GetGlbData(handle);
    REQUIRE_FALSE(result);
    REQUIRE_FALSE(assetManager.GetState(handle) == AssetManager::Status::InProgress);
}
