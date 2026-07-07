#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../../../Source/Math/Linalg/Vec2.hpp"

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
    REQUIRE(v1.Dot(v2) == 11);
}

TEST_CASE("Vec2 length works", "[math][linalg][vec2]")
{
    Vec2 v1{1.0f, 2.0f};
    REQUIRE_THAT(v1.Length(), Catch::Matchers::WithinAbs(2.23606798f, 0.0001f));

    Vec2 v2{3.0f, 4.0f};
    REQUIRE_THAT(v2.Length(), Catch::Matchers::WithinAbs(5.0f, 0.0001f));
}
