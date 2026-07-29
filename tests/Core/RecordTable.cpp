#include <catch2/catch_test_macros.hpp>

#include "../../Source/Core/RecordTable.hpp"

TEST_CASE("RecordTable holds single record", "[Core][JobSystem]") {
    Pretop::Core::RecordTable<int> table; 
    Pretop::Core::Handle handle = table.Add(1);
    int *result = table[handle];
    REQUIRE(*result == 1);
}
