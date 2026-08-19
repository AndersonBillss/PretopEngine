#include <catch2/catch_test_macros.hpp>

#include "../../source/Core/RecordTable.hpp"
#include "../../source/Core/Assert.hpp"

TEST_CASE("RecordTable holds single record", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    Pretop::Core::Handle handle = table.Add(1);
    REQUIRE(*table[handle] == 1);
    table.Release(handle);
}

TEST_CASE("RecordTable holds multiple records", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    Pretop::Core::Handle handle1 = table.Add(1);
    Pretop::Core::Handle handle2 = table.Add(2);
    Pretop::Core::Handle handle3 = table.Add(3);
    REQUIRE(*table[handle1] == 1);
    REQUIRE(*table[handle2] == 2);
    REQUIRE(*table[handle3] == 3);
    table.Release(handle1);
    table.Release(handle2);
    table.Release(handle3);
}

TEST_CASE("Freed records in RecordTable are invalid", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    Pretop::Core::Handle handle = table.Add(1);
    REQUIRE(table.IsValid(handle));
    table.Release(handle);
    REQUIRE_FALSE(table.IsValid(handle));
}

TEST_CASE("Records can be added after other records are freed", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    Pretop::Core::Handle handle1 = table.Add(1);
    Pretop::Core::Handle handle2 = table.Add(2);
    table.Release(handle2);
    Pretop::Core::Handle handle3 = table.Add(3);

    REQUIRE(table.IsValid(handle1));
    REQUIRE_FALSE(table.IsValid(handle2));
    REQUIRE(table.IsValid(handle3));
    REQUIRE(*table[handle1] == 1);
    REQUIRE(*table[handle3] == 3);
    table.Release(handle1);
}
