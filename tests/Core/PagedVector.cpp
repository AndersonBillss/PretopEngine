#include <catch2/catch_test_macros.hpp>

#include "../../source/Core/PagedVector.hpp"

using namespace Pretop::Core;

TEST_CASE("PagedVector holds single piece of data", "[Core][PagedVector]") {
    PagedVector<int> vec;
    vec.PushBack(1);
    REQUIRE(vec[0] == 1);
}

TEST_CASE("PagedVector holds multiple pieces of data", "[Core][PagedVector]") {
    PagedVector<int> vec;
    vec.PushBack(1);
    vec.PushBack(2);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
}

TEST_CASE("PagedVector holds data across multiple blocks", "[Core][PagedVector]") {
    PagedVector<int, 1> vec;
    vec.PushBack(1);
    vec.PushBack(2);
    vec.PushBack(3);
    vec.PushBack(4);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
    REQUIRE(vec[3] == 4);
}

TEST_CASE("PagedVector maintains stable data when pushing more elements", "[Core][PagedVector]") {
    PagedVector<int, 4> vec;
    vec.PushBack(1);
    for(int i = 0; i < 12; i++) {
        vec.PushBack(i);
    }
    REQUIRE(vec[0] == 1);
}

TEST_CASE("PagedVector stores size correctly", "[Core][PagedVector]") {
    PagedVector<int, 4> vec;
    for(int i = 0; i < 12; i++) {
        vec.PushBack(i);
    }
    REQUIRE(vec.Size() == 12);
}
