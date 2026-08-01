#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <ostream>

#include "Job.hpp"
#include "Handle.hpp"
#include "Status.hpp"
#include "RecordTable.hpp"

namespace Pretop::Core
{
    class JobSystem
    {
    public:
        using MainThreadFn = void (*)(JobSystem &, Pretop::Core::Handle);
        struct Completion
        {
            MainThreadFn Done = nullptr;
        };

        using Status = Pretop::Core::Status;

        JobSystem();
        ~JobSystem();

        Handle Submit(Job job);
        Handle Submit(Job job, Completion completion);

        Status GetState(Handle handle) const;
        void *GetData(Handle handle) const;
        void Release(Handle handle);

        void PumpMainThreadCompletions();

        friend std::ostream &operator<<(std::ostream &os, JobSystem &js);

    private:
        const static uint32_t _jobStateGenerationInvalid = 0;

        struct JobRecord
        {
            std::atomic<Status> State;
            void *UserData = nullptr;
            JobSystem::Completion Completion;

            JobRecord() = default;

            JobRecord(JobRecord &&other) noexcept
                : State(other.State.load(std::memory_order_relaxed)), UserData(other.UserData), Completion(std::move(other.Completion))
            {
                other.UserData = nullptr;
            }

            JobRecord &operator=(JobRecord &&other) noexcept
            {
                if (this != &other)
                {
                    State.store(other.State.load(std::memory_order_relaxed),
                                std::memory_order_relaxed);

                    UserData = other.UserData;
                    Completion = std::move(other.Completion);

                    other.UserData = nullptr;
                }

                return *this;
            }

            JobRecord(const JobRecord &) = delete;
            JobRecord &operator=(const JobRecord &) = delete;
        };

        struct CompletionEntry
        {
            Pretop::Core::Handle Handle;
        };

        struct WorkEntry
        {
            Pretop::Core::Handle Handle;
            Pretop::Core::Job Job;
        };

        void _doJob();

        std::vector<std::thread> _threads;

        std::queue<WorkEntry> _work;
        std::mutex _workMutex;
        std::condition_variable _workAvailable;

        std::queue<CompletionEntry> _completions;
        std::mutex _completionMutex;

        std::atomic<bool> _stop{false};

        RecordTable<JobRecord> _records;
    };
    // std::ostream &operator<<(std::ostream &os, Pretop::Core::JobSystem::Status status);
    // std::ostream &operator<<(std::ostream &os, Pretop::Core::Handle handle);
}
