#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>
#include <iostream>

#include "../../Source/Core/JobSystem.hpp"

using namespace Pretop::Core;

struct AddData
{
    int a;
    int b;
    int result;
};
void Add(void *data)
{
    AddData *addData = reinterpret_cast<AddData *>(data);
    addData->result = addData->a + addData->b;
}

TEST_CASE("JobSystem with one job works", "[Core][JobSystem]")
{
    JobSystem jobSystem;
    AddData *addData = new AddData{1, 2, 0};
    Job job{Add, addData};
    Handle handle = jobSystem.Submit(job);
    jobSystem.GetState(handle);

    while (jobSystem.GetState(handle) == JobSystem::JobState::InProgress)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        jobSystem.PumpMainThreadCompletions();
    }

    REQUIRE(addData->result == 3);
    delete addData;
}