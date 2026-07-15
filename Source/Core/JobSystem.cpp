#include "JobSystem.hpp"

namespace Pretop::Core
{
    JobSystem::JobSystem()
    {
        uint32_t workerCount = std::thread::hardware_concurrency();
        if (workerCount == 0)
        {
            workerCount = 1;
        }
        if (workerCount > 1)
        {
            workerCount -= 1;
        }

        for (uint32_t i = 0; i < workerCount; i++)
        {
            _threads.push_back(std::thread([&]()
                                           { this->_doJob(); }));
        }
    }
    JobSystem::~JobSystem()
    {
    }
    Handle JobSystem::Submit(Job job)
    {
        Handle handle = _addJobRecord(job);
        {
            std::lock_guard lock(_jobMutex);
            _jobs.push(std::move(job));
        }
        _jobAvailable.notify_one();
        return handle;
    }
    Handle JobSystem::Submit(Job job, Completion completion)
    {
    }
    JobSystem::JobState JobSystem::GetState(Handle handle) const
    {
        return JobState();
    }
    void JobSystem::PumpMainThreadCompletions()
    {
    }
    void JobSystem::_doJob()
    {
    }
    Handle JobSystem::_addJobRecord(const Job &job)
    {
        constexpr uint32_t kStartingGeneration = 1;

        Handle handle{};
        int staleHandleIndex = _findStaleHandle();

        if (staleHandleIndex == -1)
        {
            handle.index = static_cast<uint32_t>(_jobRecords.size());
            handle.generation = kStartingGeneration;

            _jobRecords.emplace_back(JobRecord{
                JobState::InProgress,
                kStartingGeneration,
                job.userData,
            });
        }
        else
        {
            auto &record = _jobRecords[staleHandleIndex];

            uint32_t newGeneration = record.generation + 1;
            if (newGeneration == _jobStateGenerationInvalid)
                newGeneration = kStartingGeneration;

            record.state = JobState::InProgress;
            record.userData = job.userData;
            record.generation = newGeneration;

            handle.index = static_cast<uint32_t>(staleHandleIndex);
            handle.generation = newGeneration;
        }

        return handle;
    }
    JobSystem::JobRecord *JobSystem::_getRecord(Handle handle)
    {
        if (handle.generation == 0)
        {
            return nullptr;
        }
        if (_jobRecords[handle.index].generation != handle.generation)
        {
            return nullptr;
        }
        return &_jobRecords[handle.index];
    }
    void JobSystem::_releaseJobRecord(Handle handle)
    {
    }
    int JobSystem::_findStaleHandle()
    {
        for (uint32_t i = 0; i < _jobRecords.size(); i++)
        {
            if (_jobRecords[i].generation == _jobStateGenerationInvalid)
            {
                return i;
            }
        }
        return -1;
    }
}
