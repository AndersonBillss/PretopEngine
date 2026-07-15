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
        return _getRecord(handle)->State;
    }

    void JobSystem::PumpMainThreadCompletions()
    {
        for (uint32_t i = 0; i < _jobRecords.size(); i++)
        {
            const auto &record = _jobRecords[i];
            if (
                record.State == JobState::Ready && record.Completion.Done != nullptr && _isValid(record))
            {
                record.Completion.Done(_createHandle(i), record.UserData);
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
            handle.Index = static_cast<uint32_t>(_jobRecords.size());
            handle.Generation = kStartingGeneration;

            _jobRecords.emplace_back(JobRecord{
                JobState::InProgress,
                kStartingGeneration,
                job.UserData,
                completion});
        }
        else
        {
            auto &record = _jobRecords[staleHandleIndex];

            uint32_t newGeneration = record.Generation + 1;
            if (newGeneration == _jobStateGenerationInvalid)
                newGeneration = kStartingGeneration;

            record.State = JobState::InProgress;
            record.UserData = job.UserData;
            record.Generation = newGeneration;
            record.Completion = completion;

            handle.Index = static_cast<uint32_t>(staleHandleIndex);
            handle.Generation = newGeneration;
        }

        return handle;
    }

    const JobSystem::JobRecord *JobSystem::_getRecord(Handle handle) const
    {
        if (!_isValid(handle))
        {
            return nullptr;
        }
        if (_jobRecords[handle.Index].Generation != handle.Generation)
        {
            return nullptr;
        }
        return &_jobRecords[handle.Index];
    }

    void JobSystem::_releaseJobRecord(Handle handle)
    {
        if (handle.Index >= _jobRecords.size())
        {
            return;
        }
        _jobRecords[handle.Index].Generation = _jobStateGenerationInvalid;
    }

    int JobSystem::_findStaleHandle() const
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

    bool JobSystem::_isValid(const JobRecord &record) const
    {
        return record.Generation != _jobStateGenerationInvalid;
    }

    bool JobSystem::_isValid(const Handle &handle) const
    {
        return handle.Generation != _jobStateGenerationInvalid;
    }

    Handle JobSystem::_createHandle(uint32_t handleIndex) const
    {
        Handle handle;
        handle.Index = handleIndex;
        handle.Generation = _jobRecords[handleIndex].Generation;
    }
}
