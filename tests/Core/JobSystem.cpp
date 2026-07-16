#include <catch2/catch_test_macros.hpp>
#include "../../Source/Core/JobSystem.hpp"

TEST_CASE("JobSystem with one job works", "[Core][JobSystem]")
{
    REQUIRE(1 == 2);
}