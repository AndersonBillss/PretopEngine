#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Job.hpp"
#include "Completion.hpp"
#include "Handle.hpp"

namespace Pretop::Core
{
    class JobSystem
    {
    public:
        enum class JobState
        {
            InProgress,
            Ready,
            Error,
        };

        JobSystem();
        ~JobSystem();

        Handle Submit(Job job);
        Handle Submit(Job job, Completion completion);

        JobState GetState(Handle handle) const;
        void PumpMainThreadCompletions();

    private:
        const static uint32_t _jobStateGenerationInvalid = 0;
        struct JobRecord
        {
            std::atomic<JobState> State{JobState::InProgress};
            uint32_t Generation = 0;
            void *userData;
            Completion Completion;
        };
        void _doJob();
        Handle _addJobRecord(const Job &job, const Completion &completion);
        JobRecord *_getRecord(Handle handle);
        void _releaseJobRecord(Handle handle);
        int _findStaleHandle();
        bool _isValid(const JobRecord &record);
        bool _isValid(const Handle &handle);
        Handle _createHandle(uint32_t handleIndex);

        struct CompletionEntry
        {
            Handle job;
            Completion completion;
        };

        std::vector<std::thread> _threads;

        std::queue<Job> _jobs;
        std::mutex _jobMutex;
        std::condition_variable _jobAvailable;

        std::queue<CompletionEntry> _completions;
        std::mutex _completionMutex;

        std::vector<JobRecord> _jobRecords;
    };
}
