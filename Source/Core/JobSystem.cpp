#include <magic_enum/magic_enum.hpp>
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
        std::atomic<JobState> *jobState = _getRecord(handle)->State.get();
        {
            std::lock_guard lock(_workMutex);
            WorkEntry workEntry{
                handle,
                job,
                completion,
                jobState,
            };
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
        return _getRecord(handle)->State->load();
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
            if (!record || !_isValid(record))
                continue;

            if (completion.Completion.Done)
            {
                completion.Completion.Done(completion.Handle, record->UserData);
            }

            record->State->store(JobState::Ready);
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
                work.State->store(JobState::Ready);
            }
            catch (...)
            {
                work.State->store(JobState::Error);
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

            _jobRecords.emplace_back();
            auto &record = _jobRecords.back();
            record.State = std::make_unique<std::atomic<JobState>>(JobState::InProgress);
            record.Generation = kStartingGeneration;
            record.UserData = job.UserData;
        }
        else
        {
            auto &record = _jobRecords[staleHandleIndex];

            uint32_t newGeneration = record.Generation + 1;
            if (newGeneration == _jobStateGenerationInvalid)
                newGeneration = kStartingGeneration;

            record.State->store(JobState::InProgress);
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
            if (!_isValid(&_jobRecords[i]))
            {
                return i;
            }
        }
        return -1;
    }

    bool JobSystem::_isValid(const JobRecord *record) const
    {
        return record->Generation != _jobStateGenerationInvalid;
    }

    bool JobSystem::_isValid(Handle handle) const
    {
        return handle.Generation != _jobStateGenerationInvalid;
    }

    Handle JobSystem::_createHandle(uint32_t handleIndex) const
    {
        return Handle{handleIndex, _jobRecords[handleIndex].Generation};
    }

    std::ostream &operator<<(std::ostream &os, JobSystem &js)
    {
        std::queue<JobSystem::WorkEntry> workCopy;
        {
            std::lock_guard lock(js._workMutex);
            workCopy = js._work;
        }
        std::queue<JobSystem::CompletionEntry> completionsCopy;
        {
            std::lock_guard lock(js._completionMutex);
            completionsCopy = js._completions;
        }

        os << "Job Records:\n";
        for (uint32_t i = 0; i < js._jobRecords.size(); i++)
        {
            const auto &record = js._jobRecords[i];
            os << "  " << i << " - Generation: " << record.Generation
               << ", Status: " << record.State->load() << "\n";
        }

        os << "Queued jobs:\n";

        uint32_t jobOrder = 0;
        while (!workCopy.empty())
        {
            const auto &workItem = workCopy.front();
            workCopy.pop();
            os << "  " << jobOrder << " - Handle: " << workItem.Handle << "\n";
            jobOrder++;
        }
        os << "Completions queued:\n";
        uint32_t completionsOrder = 0;
        while (!completionsCopy.empty())
        {
            const auto &completionsItem = completionsCopy.front();
            completionsCopy.pop();
            os << "  " << completionsOrder << " - Handle: " << completionsItem.Handle << "\n";
            completionsOrder++;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, Pretop::Core::JobSystem::JobState js)
    {
        os << magic_enum::enum_name(js);
        return os;
    }
    std::ostream &operator<<(std::ostream &os, Pretop::Core::Handle handle)
    {
        os << "{ " << "Index: " << handle.Index << ", " << "Generation: " << handle.Generation << " }";
        return os;
    }
}
