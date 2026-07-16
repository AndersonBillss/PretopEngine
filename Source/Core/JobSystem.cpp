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
        _stop = false;
    }

    JobSystem::~JobSystem()
    {
        _stop = true;
        _workAvailable.notify_all();

        for (auto &thread : _threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    Handle JobSystem::Submit(Job job)
    {
        Completion completion{nullptr};
        Handle handle = _addJobRecord(job, completion);
        {
            std::lock_guard lock(_workMutex);
            WorkEntry workEntry{
                handle,
                job,
                completion};
            _work.push(workEntry);
        }
        _workAvailable.notify_one();
        return handle;
    }

    Handle JobSystem::Submit(Job job, Completion completion)
    {
        Handle handle = _addJobRecord(job, completion);
        {
            std::lock_guard lock(_workMutex);
            WorkEntry workEntry{
                handle,
                job,
                completion};
            _work.push(workEntry);
        }
        _workAvailable.notify_one();
        return handle;
    }

    JobSystem::JobState JobSystem::GetState(Handle handle) const
    {
        return _getRecord(handle)->State;
    }

    void *JobSystem::GetData(Handle handle) const
    {
        if (!_isValid(handle))
        {
            return nullptr;
        }
        return _getRecord(handle)->UserData;
    }

    void JobSystem::Release(Handle handle)
    {
        _releaseJobRecord(handle);
    }

    void JobSystem::PumpMainThreadCompletions()
    {
        std::queue<CompletionEntry> pending;

        {
            std::lock_guard lock(_completionMutex);
            std::swap(pending, _completions);
        }

        while (!pending.empty())
        {
            CompletionEntry completion = pending.front();
            pending.pop();

            JobRecord *record = _getRecord(completion.Handle);
            if (!record || !_isValid(*record))
                continue;

            if (completion.Completion.Done)
            {
                record->State = JobState::Ready;
                completion.Completion.Done(completion.Handle, record->UserData);
            }

            record->State = JobState::Ready;
        }
    }

    void JobSystem::_doJob()
    {
        WorkEntry work;

        while (true)
        {
            {
                std::unique_lock lock(_workMutex);
                _workAvailable.wait(lock, [&]
                                    { return _stop || !_work.empty(); });

                if (_stop && _work.empty())
                    break;

                work = std::move(_work.front());
                _work.pop();
            }

            try
            {
                work.Job.Fn(work.Job.UserData);
            }
            catch (...)
            {
            }

            {
                std::lock_guard lock(_completionMutex);
                _completions.push(CompletionEntry{work.Handle, work.Completion});
            }
        }
    }

    Handle JobSystem::_addJobRecord(const Job &job, const Completion &completion)
    {
        uint32_t kStartingGeneration = _jobStateGenerationInvalid + 1;

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
            });
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

    JobSystem::JobRecord *JobSystem::_getRecord(Handle handle)
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

    bool JobSystem::_isValid(Handle handle) const
    {
        return handle.Generation != _jobStateGenerationInvalid;
    }

    Handle JobSystem::_createHandle(uint32_t handleIndex) const
    {
        return Handle{handleIndex, _jobRecords[handleIndex].Generation};
    }
}
