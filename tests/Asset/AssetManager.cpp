#include <catch2/catch_test_macros.hpp>

#include "../../Source/Asset/AssetLoader.hpp"
#include "../../Source/Asset/AssetManager.hpp"
#include "../../Source/Core/JobSystem.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace Pretop::Asset;
using namespace Pretop::Core;

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

    class TestAssetLoader final : public AssetLoader
    {
    public:
        void ReadBinaryAsync(
            std::string_view path,
            BinaryLoadCallback callback) override
        {
            try
            {
                callback(AssetResult<AssetBytes>{ReadBytes(std::string(path)), {}});
            }
            catch (const std::exception &error)
            {
                callback(AssetResult<AssetBytes>{AssetBytes{}, error.what()});
            }
        }

        AssetHandle<AssetText> LoadTextAsync(std::string_view) override
        {
            return {};
        }
    };
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
    AssetManager assetManager(jobSystem, std::make_unique<TestAssetLoader>());

    auto plant = assetManager.LoadModelAsync(
        AssetPath("DiffuseTransmissionPlant.glb"));
    auto mammoth = assetManager.LoadModelAsync(
        AssetPath("woolly-mammoth-100k-4096_std.glb"));

    plant.Wait();
    mammoth.Wait();

    const auto plantResult = plant.Get();
    const auto mammothResult = mammoth.Get();

    REQUIRE(plant.Kind() == AssetKind::Model);
    REQUIRE(mammoth.Kind() == AssetKind::Model);
    REQUIRE(plantResult);
    REQUIRE(mammothResult);
    REQUIRE_FALSE(plantResult.Data.Vertices.empty());
    REQUIRE_FALSE(mammothResult.Data.Vertices.empty());
}

TEST_CASE("AssetManager reports byte loading errors", "[Asset][AssetManager]")
{
    JobSystem jobSystem;
    AssetManager assetManager(jobSystem, std::make_unique<TestAssetLoader>());

    auto model = assetManager.LoadModelAsync(AssetPath("missing.glb"));
    model.Wait();

    const auto result = model.Get();
    REQUIRE_FALSE(result);
    REQUIRE_FALSE(result.Error.empty());
}
