#include <catch2/catch_test_macros.hpp>
#include "../matchers/Vec4ApproxMatcher.hpp"
#include "../../src/math/euler.hpp"
#include "../../src/math/constants.hpp"

TEST_CASE("X euler rotate basis vectors as expected", "[math][euler]")
{
    Euler e{90.0f * (float)deg2rad, 0, 0};
    Vec4 v1{1, 0, 0, 0};
    Vec4 result1{1, 0, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v1, Vec4ApproxMatcher(result1, 1e-5f));

    Vec4 v2{0, 1, 0, 0};
    Vec4 result2{0, 0, 1, 0};
    REQUIRE_THAT(e.toMatrix() * v2, Vec4ApproxMatcher(result2, 1e-5f));

    Vec4 v3{0, 0, 1, 0};
    Vec4 result3{0, -1, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v3, Vec4ApproxMatcher(result3, 1e-5f));
}

TEST_CASE("Y euler rotate basis vectors as expected", "[math][euler]")
{
    Euler e{0, 90.0f * (float)deg2rad, 0};
    Vec4 v1{1, 0, 0, 0};
    Vec4 result1{0, 0, -1, 0};
    REQUIRE_THAT(e.toMatrix() * v1, Vec4ApproxMatcher(result1, 1e-5f));

    Vec4 v2{0, 1, 0, 0};
    Vec4 result2{0, 1, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v2, Vec4ApproxMatcher(result2, 1e-5f));

    Vec4 v3{0, 0, 1, 0};
    Vec4 result3{1, 0, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v3, Vec4ApproxMatcher(result3, 1e-5f));
}

TEST_CASE("Z euler rotate basis vectors as expected", "[math][euler]")
{
    Euler e{0, 0, 90.0f * (float)deg2rad};
    Vec4 v1{1, 0, 0, 0};
    Vec4 result1{0, 1, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v1, Vec4ApproxMatcher(result1, 1e-5f));

    Vec4 v2{0, 1, 0, 0};
    Vec4 result2{-1, 0, 0, 0};
    REQUIRE_THAT(e.toMatrix() * v2, Vec4ApproxMatcher(result2, 1e-5f));

    Vec4 v3{0, 0, 1, 0};
    Vec4 result3{0, 0, 1, 0};
    REQUIRE_THAT(e.toMatrix() * v3, Vec4ApproxMatcher(result3, 1e-5f));
}
