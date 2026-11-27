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

TEST_CASE("Records iterator works", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    table.Add(1);
    table.Add(2);
    table.Add(3);

    int i = 1;
    for (Pretop::Core::Handle handle : table)
    {
        REQUIRE(*table[handle] == i);
        i++;
    }
}

TEST_CASE("Records iterator skips over removed records", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    table.Add(1);
    table.Add(2);
    Pretop::Core::Handle handle = table.Add(3);
    table.Add(4);

    table.Release(handle);

    std::vector<int> expected = {1, 2, 4};

    uint32_t i = 0;
    for (Pretop::Core::Handle handle : table)
    {
        REQUIRE(*table[handle] == expected[i]);
        i++;
    }
}

TEST_CASE("Records iterator skips over removed value in last index", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    table.Add(1);
    table.Add(2);
    Pretop::Core::Handle handle = table.Add(3);

    table.Release(handle);

    std::vector<int> expected = {1, 2};

    uint32_t i = 0;
    for (Pretop::Core::Handle handle : table)
    {
        REQUIRE(i < expected.size());
        REQUIRE(*table[handle] == expected[i]);
        i++;
    }
}

TEST_CASE("Records iterator skips over multiple removed values", "[Core][RecordTable]")
{
    Pretop::Core::RecordTable<int> table;
    Pretop::Core::Handle handle1 = table.Add(15);
    table.Add(1);
    table.Add(2);
    Pretop::Core::Handle handle2 = table.Add(3);
    table.Add(9);
    Pretop::Core::Handle handle3 = table.Add(8);
    table.Add(12);
    table.Add(13);
    table.Add(10);
    table.Add(5);
    Pretop::Core::Handle handle4 = table.Add(4);

    std::vector<int> expected1 = {15, 1, 2, 3, 9, 8, 12, 13, 10, 5, 4};
    uint32_t i = 0;
    for (Pretop::Core::Handle handle : table)
    {
        REQUIRE(i < expected1.size());
        REQUIRE(*table[handle] == expected1[i]);
        i++;
    }

    table.Release(handle1);
    table.Release(handle2);
    table.Release(handle3);
    table.Release(handle4);
    std::vector<int> expected2 = {1, 2, 9, 12, 13, 10, 5};

    i = 0;
    for (Pretop::Core::Handle handle : table)
    {
        REQUIRE(i < expected2.size());
        REQUIRE(*table[handle] == expected2[i]);
        i++;
    }
}
