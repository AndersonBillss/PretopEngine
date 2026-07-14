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
        void _doJob();
        Handle _addJobRecord(Job job);
        void _releaseJobRecord(Handle handle);
        int _findStaleHandle();

        const static uint32_t _jobStateGenerationInvalid = 0;
        struct JobRecord
        {
            std::atomic<JobState> state{JobState::InProgress};
            uint32_t generation = 0;
            Job job;
        };

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
