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
        void Dispatch(Job job);

        Status GetState(Handle handle) const;
        void *GetData(Handle handle) const;
        void Release(Handle handle);

        void PumpMainThreadCompletions();

        friend std::ostream &operator<<(std::ostream &os, JobSystem &js);

    private:
        const static uint32_t _jobStateGenerationInvalid = 0;

        struct JobRecord
        {
            std::unique_ptr<std::atomic<Status>> State;
            uint32_t Generation = 0;
            void *UserData = nullptr;
            JobSystem::Completion Completion;
        };

        struct CompletionEntry
        {
            Pretop::Core::Handle Handle;
            JobSystem::Completion Completion;
        };

        struct WorkEntry
        {
            Pretop::Core::Handle Handle;
            Pretop::Core::Job Job;
            JobSystem::Completion Completion;
            std::atomic<Status> *State;
        };

        void _doJob();

        Handle _addJobRecord(const Job &job, const Completion &completion);
        const JobRecord *_getRecord(Handle handle) const;
        JobRecord *_getRecord(Handle handle);
        void _releaseJobRecord(Handle handle);

        Handle _createHandle(uint32_t handleIndex) const;
        int _findStaleHandle() const;

        bool _isValid(const JobRecord &record) const;
        bool _isValid(Handle handle) const;

        std::vector<std::thread> _threads;

        std::queue<WorkEntry> _work;
        std::mutex _workMutex;
        std::condition_variable _workAvailable;

        std::queue<CompletionEntry> _completions;
        std::mutex _completionMutex;

        std::atomic<bool> _stop{false};

        std::vector<JobRecord> _jobRecords;
    };
    std::ostream &operator<<(std::ostream &os, Pretop::Core::JobSystem::Status status);
    std::ostream &operator<<(std::ostream &os, Pretop::Core::Handle handle);
}
