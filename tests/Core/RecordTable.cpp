#include <catch2/catch_test_macros.hpp>

#include "../../Source/Core/RecordTable.hpp"
#include "../../Source/Core/Assert.hpp"

TEST_CASE("RecordTable holds single record", "[Core][RecordTable]") {
    Pretop::Core::RecordTable<int> table; 
    Pretop::Core::Handle handle = table.Add(1);
    REQUIRE(*table[handle] == 1);
}

TEST_CASE("RecordTable holds multiple records", "[Core][RecordTable]") {
    Pretop::Core::RecordTable<int> table; 
    Pretop::Core::Handle handle1 = table.Add(1);
    Pretop::Core::Handle handle2 = table.Add(2);
    Pretop::Core::Handle handle3 = table.Add(3);
    REQUIRE(*table[handle1] == 1);
    REQUIRE(*table[handle2] == 2);
    REQUIRE(*table[handle3] == 3);
}

TEST_CASE("Freed records in RecordTable are invalid", "[Core][RecordTable]") {
    Pretop::Core::RecordTable<int> table; 
    Pretop::Core::Handle handle = table.Add(1);
    REQUIRE(table.IsValid(handle));
    table.Release(handle);
    REQUIRE_FALSE(table.IsValid(handle));
}
