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
        return Handle();
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
    Handle JobSystem::_addJobRecord(Job job)
    {
        uint32_t startingGeneration = 1;
        Handle handle;
        int staleHandleIndex = _findStaleHandle();
        if (staleHandleIndex == -1)
        {
            handle.generation = startingGeneration;
            handle.index = _jobRecords.size();
            _jobRecords.push_back(JobRecord{
                JobState::InProgress,
                startingGeneration,
                job,
            });
        }
        else
        {
            uint32_t newGeneration = _jobRecords[staleHandleIndex].generation + 1;
            if (newGeneration == _jobStateGenerationInvalid)
            {
                newGeneration = startingGeneration;
            }
            handle.generation = newGeneration;
            handle.index = staleHandleIndex;
            _jobRecords[staleHandleIndex].state = JobState::InProgress;
            _jobRecords[staleHandleIndex].job = job;
            _jobRecords[staleHandleIndex].generation = newGeneration;
        }
        return handle;
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
