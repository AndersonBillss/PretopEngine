#include <magic_enum/magic_enum.hpp>
#include <iostream>
#include "JobSystem.hpp"
#include "Assert.hpp"

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
        return Submit(job, completion);
    }

    Handle JobSystem::Submit(Job job, Completion completion)
    {
        JobRecord record;
        record.Completion = completion;
        record.State.store(Status::InProgress);
        record.UserData = job.UserData;

        Handle handle = _records.Add(std::move(record));
        {
            std::lock_guard lock(_workMutex);
            WorkEntry workEntry{
                /* Handle */ handle,
                /* Job */ job,
            };
            _work.push(workEntry);
        }
        _workAvailable.notify_one();
        return handle;
    }

    JobSystem::Status JobSystem::GetState(Handle handle) const
    {
        return _records[handle]->State.load();
    }

    void *JobSystem::GetData(Handle handle) const
    {
        return _records[handle]->UserData;
    }

    void JobSystem::Release(Handle handle)
    {
        _records.Release(handle);
    }

    void JobSystem::PumpMainThreadCompletions()
    {
        std::queue<CompletionEntry> pending;

        {
            std::lock_guard lock(_completionMutex);
            if (!_completions.size())
                return;
            std::swap(pending, _completions);
        }

        while (!pending.empty())
        {
            CompletionEntry completion = pending.front();
            pending.pop();

            JobRecord *record = _records[completion.Handle];
            record->Completion.Done(*this, completion.Handle);

            if (record->State == Status::InProgress)
            {
                record->State.store(Status::Ready);
            }
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

            JobRecord *record = _records[work.Handle];
            try
            {
                work.Job.Fn(work.Job.UserData);
                record->State.store(Status::Ready);
            }
            catch (...)
            {
                record->State.store(Status::Error);
            }

            {
                std::lock_guard lock(_completionMutex);
                _completions.push(CompletionEntry{work.Handle});
            }
        }
    }
}
