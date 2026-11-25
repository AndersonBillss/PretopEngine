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
            ++_readCount;
            _rawBytesCb = rawBytesCb;
            _finishCb = finishCb;
            _userData = userData;
            _assetExists = true;
            return _handle;
        }

        Status GetStatus(Handle) const override
        {
            if (!_assetExists)
            {
                ++_statusQueriesAfterRelease;
            }
            return _status;
        }
        const AssetBytes &GetBytes(Handle) const override { return _bytes; }
        void *GetRawData(Handle) override { return _userData; }
        std::string GetError(Handle) const override
        {
            if (!_assetExists)
            {
                ++_errorQueriesAfterRelease;
                return {};
            }
            return _errorMessage;
        }

        void Release(Handle) override
        {
            ++_releaseCount;
            _assetExists = false;
            _bytes.clear();
            _userData = nullptr;
            _errorMessage.clear();
        }

        void CompleteLoad()
        {
            _rawBytesCb(_bytes, _userData);
            _status = Status::Ready;
            _finishCb(*this, _handle);
        }

        void FailLoad(std::string errorMessage)
        {
            _errorMessage = std::move(errorMessage);
            _status = Status::Error;
            _finishCb(*this, _handle);
        }

        bool AssetExists() const { return _assetExists; }
        bool HasBytes() const { return !_bytes.empty(); }
        int ReadCount() const { return _readCount; }
        int ReleaseCount() const { return _releaseCount; }
        int StatusQueriesAfterRelease() const { return _statusQueriesAfterRelease; }
        int ErrorQueriesAfterRelease() const { return _errorQueriesAfterRelease; }

    private:
        const Handle _handle{0, 1};
        AssetBytes _bytes{std::byte{42}};
        Status _status = Status::InProgress;
        RawBytesCb _rawBytesCb = nullptr;
        FinishCb _finishCb = nullptr;
        void *_userData = nullptr;
        bool _assetExists = false;
        int _readCount = 0;
        int _releaseCount = 0;
        std::string _errorMessage;
        mutable int _statusQueriesAfterRelease = 0;
        mutable int _errorQueriesAfterRelease = 0;
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
        explicit TestAssetBuilder(int &destructionCount, int *finalizeCount = nullptr)
            : _destructionCount(destructionCount), _finalizeCount(finalizeCount) {}

        void ThreadLoadStep(const AssetBytes &bytes) override
        {
            REQUIRE(bytes.size() == 1);
            REQUIRE(bytes[0] == std::byte{42});
        }

        AssetType GetAssetType() override { return AssetType::Texture; }

        std::unique_ptr<AssetData> FinalizeLoadStep() override
        {
            if (_finalizeCount != nullptr)
            {
                ++*_finalizeCount;
            }
            return std::make_unique<TestAssetData>(_destructionCount);
        }

    private:
        int &_destructionCount;
        int *_finalizeCount;
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

TEST_CASE("AssetManager loads a shared asset once and frees it after all references are released", "[Asset][AssetManager]")
{
    int assetDataDestructionCount = 0;
    auto loader = std::make_unique<MockAssetLoader>();
    MockAssetLoader *loaderView = loader.get();
    Pretop::Asset::AssetManager manager(std::move(loader), {});

    auto firstReference = manager.LoadAsset(17, std::make_unique<TestAssetBuilder>(assetDataDestructionCount));
    auto secondReference = manager.LoadAsset(17, std::make_unique<TestAssetBuilder>(assetDataDestructionCount));

    REQUIRE(loaderView->ReadCount() == 1);
    loaderView->CompleteLoad();

    void *firstAssetData = nullptr;
    void *secondAssetData = nullptr;
    REQUIRE(manager.GetAsset(firstReference, Pretop::Asset::AssetType::Texture, &firstAssetData) ==
            Pretop::Asset::AssetManager::AssetResult::Success);
    REQUIRE(manager.GetAsset(secondReference, Pretop::Asset::AssetType::Texture, &secondAssetData) ==
            Pretop::Asset::AssetManager::AssetResult::Success);
    CHECK(firstAssetData == secondAssetData);
    CHECK(assetDataDestructionCount == 0);

    manager.Release(firstReference);

    CHECK(manager.GetAsset(firstReference, Pretop::Asset::AssetType::Texture, &firstAssetData) ==
          Pretop::Asset::AssetManager::AssetResult::InvalidReference);
    REQUIRE(manager.GetAsset(secondReference, Pretop::Asset::AssetType::Texture, &secondAssetData) ==
            Pretop::Asset::AssetManager::AssetResult::Success);
    CHECK(assetDataDestructionCount == 0);

    manager.Release(secondReference);

    CHECK(manager.GetAsset(secondReference, Pretop::Asset::AssetType::Texture, &secondAssetData) ==
          Pretop::Asset::AssetManager::AssetResult::InvalidReference);
    CHECK(loaderView->ReadCount() == 1);
    CHECK(loaderView->ReleaseCount() == 1);
    CHECK(assetDataDestructionCount == 1);
}

TEST_CASE("CopyReference creates an independent reference to the same asset", "[Asset][AssetManager]")
{
    int assetDataDestructionCount = 0;
    auto loader = std::make_unique<MockAssetLoader>();
    MockAssetLoader *loaderView = loader.get();
    Pretop::Asset::AssetManager manager(std::move(loader), {});

    Pretop::Asset::AssetManager::UntypedAssetReference original = manager.LoadAsset(
        23,
        std::make_unique<TestAssetBuilder>(assetDataDestructionCount));
    Pretop::Asset::AssetManager::UntypedAssetReference copy = manager.CopyReference(original);
    // CHECK(copy.handle.Index != original.handle.Index || copy.handle.Generation != original.handle.Generation);

    loaderView->CompleteLoad();

    void *originalData = nullptr;
    void *copyData = nullptr;
    REQUIRE(manager.GetAsset(original, Pretop::Asset::AssetType::Texture, &originalData) ==
            Pretop::Asset::AssetManager::AssetResult::Success);
    REQUIRE(manager.GetAsset(copy, Pretop::Asset::AssetType::Texture, &copyData) ==
            Pretop::Asset::AssetManager::AssetResult::Success);
    CHECK(originalData == copyData);

    manager.Release(original);

    CHECK(manager.GetAsset(original, Pretop::Asset::AssetType::Texture, &originalData) ==
          Pretop::Asset::AssetManager::AssetResult::InvalidReference);
    REQUIRE(assetDataDestructionCount == 0);
    REQUIRE(manager.GetAsset(copy, Pretop::Asset::AssetType::Texture, &copyData) ==
            Pretop::Asset::AssetManager::AssetResult::Success);
    CHECK(copyData == originalData);

    manager.Release(copy);

    CHECK(manager.GetAsset(copy, Pretop::Asset::AssetType::Texture, &copyData) ==
          Pretop::Asset::AssetManager::AssetResult::InvalidReference);
    CHECK(assetDataDestructionCount == 1);
}

TEST_CASE("AssetManager reports not-ready and successful load statuses", "[Asset][AssetManager]")
{
    int assetDataDestructionCount = 0;
    auto loader = std::make_unique<MockAssetLoader>();
    MockAssetLoader *loaderView = loader.get();
    Pretop::Asset::AssetManager manager(std::move(loader), {});
    auto reference = manager.LoadAsset(31, std::make_unique<TestAssetBuilder>(assetDataDestructionCount));

    void *assetData = nullptr;
    CHECK(manager.GetResult(reference) == Pretop::Asset::AssetManager::AssetResult::NotReady);
    CHECK(manager.GetAsset(reference, Pretop::Asset::AssetType::Texture, &assetData) ==
          Pretop::Asset::AssetManager::AssetResult::NotReady);
    CHECK(manager.GetError(reference) == "Asset is not ready");

    loaderView->CompleteLoad();

    CHECK(manager.GetResult(reference) == Pretop::Asset::AssetManager::AssetResult::Success);
    CHECK(manager.GetAsset(reference, Pretop::Asset::AssetType::Texture, &assetData) ==
          Pretop::Asset::AssetManager::AssetResult::Success);
    CHECK(manager.GetError(reference).empty());
    CHECK(loaderView->StatusQueriesAfterRelease() == 0);
    CHECK(loaderView->ErrorQueriesAfterRelease() == 0);

    manager.Release(reference);
}

TEST_CASE("AssetManager reports loader failures", "[Asset][AssetManager]")
{
    constexpr const char *errorMessage = "mock asset load failed";
    int assetDataDestructionCount = 0;
    int finalizeCount = 0;
    auto loader = std::make_unique<MockAssetLoader>();
    MockAssetLoader *loaderView = loader.get();
    Pretop::Asset::AssetManager manager(std::move(loader), {});
    auto reference = manager.LoadAsset(
        32,
        std::make_unique<TestAssetBuilder>(assetDataDestructionCount, &finalizeCount));

    loaderView->FailLoad(errorMessage);

    void *assetData = nullptr;
    CHECK(manager.GetResult(reference) == Pretop::Asset::AssetManager::AssetResult::Failed);
    CHECK(manager.GetAsset(reference, Pretop::Asset::AssetType::Texture, &assetData) ==
          Pretop::Asset::AssetManager::AssetResult::Failed);
    CHECK(manager.GetError(reference) == errorMessage);
    CHECK(finalizeCount == 0);
    CHECK(loaderView->ReleaseCount() == 1);
    CHECK(loaderView->StatusQueriesAfterRelease() == 0);
    CHECK(loaderView->ErrorQueriesAfterRelease() == 0);

    manager.Release(reference);
}

TEST_CASE("AssetManager reports invalid references and type mismatches", "[Asset][AssetManager]")
{
    auto loader = std::make_unique<MockAssetLoader>();
    MockAssetLoader *loaderView = loader.get();
    Pretop::Asset::AssetManager manager(std::move(loader), {});

    Pretop::Asset::AssetManager::UntypedAssetReference invalidReference{{0, 0}};
    void *assetData = nullptr;
    CHECK(manager.GetResult(invalidReference) == Pretop::Asset::AssetManager::AssetResult::InvalidReference);
    CHECK(manager.GetAsset(invalidReference, Pretop::Asset::AssetType::Texture, &assetData) ==
          Pretop::Asset::AssetManager::AssetResult::InvalidReference);
    CHECK(manager.GetError(invalidReference) == "Asset reference is invalid");

    int assetDataDestructionCount = 0;
    auto reference = manager.LoadAsset(33, std::make_unique<TestAssetBuilder>(assetDataDestructionCount));
    loaderView->CompleteLoad();
    CHECK(manager.GetAsset(reference, Pretop::Asset::AssetType::Shader, &assetData) ==
          Pretop::Asset::AssetManager::AssetResult::WrongType);
    manager.Release(reference);
}
