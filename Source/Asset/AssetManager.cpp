#include "AssetManager.hpp"

#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"

#include <exception>
#include <memory>
#include <string>
#include <utility>

namespace Pretop::Asset
{
    namespace
    {
        using ModelResult = AssetResult<ParsedData>;

        struct ParseModelJob
        {
            AssetBytes Bytes;
            TaskCompletion<ModelResult> Completion;
        };

        void ParseModel(void *userData)
        {
            std::unique_ptr<ParseModelJob> job(static_cast<ParseModelJob *>(userData));

            try
            {
                ParsedData model = LoadGlb(job->Bytes);
                job->Completion.SetResult(ModelResult{std::move(model), {}});
            }
            catch (const std::exception &error)
            {
                job->Completion.SetResult(ModelResult{ParsedData{}, error.what()});
            }
            catch (...)
            {
                job->Completion.SetResult(
                    ModelResult{ParsedData{}, "Unknown error while parsing model."});
            }
        }
    } // namespace

    AssetManager::AssetManager(
        Core::JobSystem &jobSystem,
        std::unique_ptr<AssetLoader> assetLoader)
        : _jobSystem(jobSystem), _assetLoader(std::move(assetLoader))
    {
    }

    AssetManager::~AssetManager() = default;

    AssetHandle<ParsedData> AssetManager::LoadModelAsync(std::string_view path)
    {
        TaskCompletion<ModelResult> completion;
        auto task = completion.CreateTask();
        const std::string id(path);
        Core::JobSystem *jobSystem = &_jobSystem;

        _assetLoader->ReadBinaryAsync(
            path,
            [jobSystem, completion = std::move(completion)](AssetResult<AssetBytes> result) mutable
            {
                if (!result)
                {
                    completion.SetResult(ModelResult{ParsedData{}, std::move(result.Error)});
                    return;
                }

                auto job = std::make_unique<ParseModelJob>();
                job->Bytes = std::move(result.Data);
                job->Completion = completion;

                try
                {
                    jobSystem->Dispatch(Core::Job{ParseModel, job.get()});
                    job.release();
                }
                catch (const std::exception &error)
                {
                    completion.SetResult(ModelResult{ParsedData{}, error.what()});
                }
                catch (...)
                {
                    completion.SetResult(
                        ModelResult{ParsedData{}, "Unknown error while scheduling model parsing."});
                }
            });

        return AssetHandle<ParsedData>{id, AssetKind::Model, std::move(task)};
    }
} // namespace Pretop::Asset
