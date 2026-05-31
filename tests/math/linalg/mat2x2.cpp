#include <catch2/catch_test_macros.hpp>
#include "../../../src/math/linalg/mat2x2.hpp"
#include "../../../src/math/linalg/vec2.hpp"

TEST_CASE("Mat2x2 addition works", "[math][linalg][mat2x2]")
{
    Mat2x2 m{1.0f, 2.0f,
             3.0f, 4.0f};
    Vec2 v{3.0f, 4.0f};
    Vec2 result{11.0f, 25.0f};
    REQUIRE(m * v == result);
}