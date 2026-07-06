#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../../../src/Math/Linalg/Vec3.hpp"

TEST_CASE("Vec3 addition works", "[math][linalg][vec3]")
{
    Vec3 v1{1, 2, 0};
    Vec3 v2{3, 4, 1};
    Vec3 result{4, 6, 1};
    REQUIRE(v1 + v2 == result);
    v1 += v2;
    REQUIRE(v1 == result);
}

TEST_CASE("Vec3 subtraction works", "[math][linalg][vec3]")
{
    Vec3 v1{1, 2, 2};
    Vec3 v2{3, 4, 0};
    Vec3 result{-2, -2, 2};
    REQUIRE(v1 - v2 == result);
    v1 -= v2;
    REQUIRE(v1 == result);
}

TEST_CASE("Vec3 dot product works", "[math][linalg][vec3]")
{
    Vec3 v1{1, 2, 5};
    Vec3 v2{3, 4, 5};
    REQUIRE(v1.dot(v2) == 36);
}

TEST_CASE("Vec3 length works", "[math][linalg][vec3]")
{
    Vec3 v1{1.0f, 2.0f, 3.0f};
    REQUIRE_THAT(v1.length(), Catch::Matchers::WithinAbs(3.74165739f, 0.0001f));

    Vec3 v2{3.0f, 4.0f, 8.0f};
    REQUIRE_THAT(v2.length(), Catch::Matchers::WithinAbs(9.43398113f, 0.0001f));
}
