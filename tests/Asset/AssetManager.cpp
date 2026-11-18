#include <catch2/catch_test_macros.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../../source/Asset/AssetManager.hpp"

namespace
{
    using namespace Pretop::Asset;

    class MockAssetLoader final : public AssetLoader
    {
    public:
        Handle ReadFile(std::string_view) override { return {}; }
        Handle ReadFile(std::string_view, FinishCb, void *) override { return {}; }
        Handle ReadFile(std::string_view, RawBytesCb, FinishCb, void *) override { return {}; }
        Handle ReadFile(uint64_t) override { return {}; }
        Handle ReadFile(uint64_t, FinishCb, void *) override { return {}; }

        Handle ReadFile(uint64_t, RawBytesCb rawBytesCb, FinishCb finishCb, void *userData) override
        {
            _rawBytesCb = rawBytesCb;
            _finishCb = finishCb;
            _userData = userData;
            _assetExists = true;
            return _handle;
        }

        Status GetStatus(Handle) const override { return _status; }
        const AssetBytes &GetBytes(Handle) const override { return _bytes; }
        void *GetRawData(Handle) override { return _userData; }
        std::string GetError(Handle) const override { return {}; }

        void Release(Handle) override
        {
            ++_releaseCount;
            _assetExists = false;
            _bytes.clear();
            _userData = nullptr;
        }

        void CompleteLoad()
        {
            _rawBytesCb(_bytes, _userData);
            _status = Status::Ready;
            _finishCb(*this, _handle);
        }

        bool AssetExists() const { return _assetExists; }
        bool HasBytes() const { return !_bytes.empty(); }
        int ReleaseCount() const { return _releaseCount; }

    private:
        const Handle _handle{0, 1};
        AssetBytes _bytes{std::byte{42}};
        Status _status = Status::InProgress;
        RawBytesCb _rawBytesCb = nullptr;
        FinishCb _finishCb = nullptr;
        void *_userData = nullptr;
        bool _assetExists = false;
        int _releaseCount = 0;
    };

    class TestAssetData final : public AssetData
    {
    public:
        explicit TestAssetData(int &destructionCount) : _destructionCount(destructionCount) {}
        ~TestAssetData() override { ++_destructionCount; }

        void *GetData() override { return &_value; }

    private:
        int &_destructionCount;
        int _value = 42;
    };

    class TestAssetBuilder final : public AssetBuilder
    {
    public:
        explicit TestAssetBuilder(int &destructionCount) : _destructionCount(destructionCount) {}

        void ThreadLoadStep(const AssetBytes &bytes) override
        {
            REQUIRE(bytes.size() == 1);
            REQUIRE(bytes[0] == std::byte{42});
        }

        AssetType GetAssetType() override { return AssetType::Texture; }

        std::unique_ptr<AssetData> FinalizeLoadStep() override
        {
            return std::make_unique<TestAssetData>(_destructionCount);
        }

    private:
        int &_destructionCount;
    };
}

TEST_CASE("AssetManager deletes a loaded asset after its final reference is released", "[Asset][AssetManager]")
{
    int assetDataDestructionCount = 0;
    auto loader = std::make_unique<MockAssetLoader>();
    MockAssetLoader *loaderView = loader.get();
    Pretop::Asset::AssetManager manager(std::move(loader), {});

    auto reference = manager.LoadAsset(17, std::make_unique<TestAssetBuilder>(assetDataDestructionCount));
    loaderView->CompleteLoad();

    void *assetData = nullptr;
    REQUIRE(manager.GetAsset(reference, Pretop::Asset::AssetType::Texture, &assetData) ==
            Pretop::Asset::AssetManager::AssetResult::Success);
    REQUIRE(*static_cast<int *>(assetData) == 42);
    CHECK_FALSE(loaderView->AssetExists());
    CHECK_FALSE(loaderView->HasBytes());
    CHECK(loaderView->ReleaseCount() == 1);
    CHECK(assetDataDestructionCount == 0);

    manager.Release(reference);

    CHECK_FALSE(loaderView->AssetExists());
    CHECK(loaderView->ReleaseCount() == 1);
    CHECK(assetDataDestructionCount == 1);
    CHECK(manager.GetAsset(reference, Pretop::Asset::AssetType::Texture, &assetData) ==
          Pretop::Asset::AssetManager::AssetResult::InvalidReference);
}
