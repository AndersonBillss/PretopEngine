#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../../../src/Math/Linalg/Vec4.hpp"

TEST_CASE("Vec4 addition works", "[math][linalg][vec4]")
{
    Vec4 v1{1, 2, 0, 3};
    Vec4 v2{3, 4, 1, 5};
    Vec4 result{4, 6, 1, 8};
    REQUIRE(v1 + v2 == result);
    v1 += v2;
    REQUIRE(v1 == result);
}

TEST_CASE("Vec4 subtraction works", "[math][linalg][vec4]")
{
    Vec4 v1{1, 2, 2, 9};
    Vec4 v2{3, 4, 0, 2};
    Vec4 result{-2, -2, 2, 7};
    REQUIRE(v1 - v2 == result);
    v1 -= v2;
    REQUIRE(v1 == result);
}

TEST_CASE("Vec4 dot product works", "[math][linalg][vec4]")
{
    Vec4 v1{1, 2, 5, 3};
    Vec4 v2{3, 4, 5, 4};
    REQUIRE(v1.dot(v2) == 48);
}

TEST_CASE("Vec4 length works", "[math][linalg][vec4]")
{
    Vec4 v1{1.0f, 2.0f, 3.0f, 5.0f};
    REQUIRE_THAT(v1.length(), Catch::Matchers::WithinAbs(6.244998f, 0.0001f));

    Vec4 v2{3.0f, 4.0f, 8.0f, 5.0f};
    REQUIRE_THAT(v2.length(), Catch::Matchers::WithinAbs(10.6770783f, 0.0001f));
}
