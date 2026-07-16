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

void WaitUntilDone(JobSystem &jobSystem, Handle handle)
{
    while (jobSystem.GetState(handle) == JobSystem::JobState::InProgress)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

TEST_CASE("JobSystem with one job works", "[Core][JobSystem]")
{
    JobSystem jobSystem;
    AddData *addData = new AddData{1, 2, 0};
    Handle handle = jobSystem.Submit(Job{Add, addData});
    WaitUntilDone(jobSystem, handle);

    REQUIRE(jobSystem.GetState(handle) == JobSystem::JobState::Ready);
    REQUIRE(addData->result == 3);

    delete addData;
}