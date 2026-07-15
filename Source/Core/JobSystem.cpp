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
        Completion completion{nullptr};
        Handle handle = _addJobRecord(job, completion);
        {
            std::lock_guard lock(_jobMutex);
            _jobs.push(std::move(job));
        }
        _jobAvailable.notify_one();
        return handle;
    }

    Handle JobSystem::Submit(Job job, Completion completion)
    {
        Handle handle = _addJobRecord(job, completion);
        {
            std::lock_guard lock(_jobMutex);
            _jobs.push(std::move(job));
        }
        _jobAvailable.notify_one();
        return handle;
    }

    JobSystem::JobState JobSystem::GetState(Handle handle) const
    {
        return JobState();
    }

    void JobSystem::PumpMainThreadCompletions()
    {
        for (uint32_t i = 0; i < _jobRecords.size(); i++)
        {
            const auto &record = _jobRecords[i];
            if (
                record.state == JobState::Ready && record.Completion.done != nullptr && _isValid(record))
            {
                record.Completion.done(_createHandle(i), record.userData);
            }
        }
    }

    void JobSystem::_doJob()
    {
    }

    Handle JobSystem::_addJobRecord(const Job &job, const Completion &completion)
    {
        uint32_t kStartingGeneration = _jobStateGenerationInvalid;

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
                completion});
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
            record.Completion = completion;

            handle.index = static_cast<uint32_t>(staleHandleIndex);
            handle.generation = newGeneration;
        }

        return handle;
    }

    JobSystem::JobRecord *JobSystem::_getRecord(Handle handle)
    {
        if (!_isValid(handle))
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
        if (handle.index >= _jobRecords.size())
        {
            return;
        }
        _jobRecords[handle.index].generation = _jobStateGenerationInvalid;
    }

    int JobSystem::_findStaleHandle()
    {
        for (uint32_t i = 0; i < _jobRecords.size(); i++)
        {
            if (!_isValid(_jobRecords[i]))
            {
                return i;
            }
        }
        return -1;
    }

    bool JobSystem::_isValid(const JobRecord &record)
    {
        return record.generation != _jobStateGenerationInvalid;
    }

    bool JobSystem::_isValid(const Handle &handle)
    {
        return handle.generation != _jobStateGenerationInvalid;
    }

    Handle JobSystem::_createHandle(uint32_t handleIndex)
    {
        Handle handle;
        handle.index = handleIndex;
        handle.generation = _jobRecords[handleIndex].generation;
    }
}
