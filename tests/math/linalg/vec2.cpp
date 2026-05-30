#include <catch2/catch_test_macros.hpp>
#include "../../../src/math/linalg/vec2.hpp"

TEST_CASE("Vec2 addition works", "[math][linalg][vec2]")
{
    Vec2 v1{1, 2};
    Vec2 v2{3, 4};
    Vec2 result{4, 6};
    REQUIRE(v1 + v2 == result);
    v1 += v2;
    REQUIRE(v1 == result);
}

TEST_CASE("Vec2 subtraction works", "[math][linalg][vec2]")
{
    Vec2 v1{1, 2};
    Vec2 v2{3, 4};
    Vec2 result{-2, -2};
    REQUIRE(v1 - v2 == result);
    v1 -= v2;
    REQUIRE(v1 == result);
}

TEST_CASE("Vec2 dot product works", "[math][linalg][vec2]")
{
    Vec2 v1{1, 2};
    Vec2 v2{3, 4};
    float result = 11;
    REQUIRE(v1.dot(v2) == result);
}