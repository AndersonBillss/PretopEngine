#include <catch2/catch_test_macros.hpp>
#include "../../../src/math/linalg/mat2x2.hpp"

TEST_CASE("Mat2x2 Vec2 multiplication works", "[math][linalg][mat2x2]")
{
    Mat2x2 m{1.0f, 2.0f,
             3.0f, 4.0f};
    Vec2 v{3.0f, 4.0f};
    Vec2 result{11.0f, 25.0f};
    REQUIRE(m * v == result);
}

TEST_CASE("Mat2x2 Mat2x2 multiplication works", "[math][linalg][mat2x2]")
{
    Mat2x2 m1{1.0f, 2.0f,
              3.0f, 4.0f};
    Mat2x2 m2{3.0f, 1.0f,
              2.0f, 5.0f};
    Mat2x2 result{7.0f, 11.0f,
                  17.0f, 23.0f};
    REQUIRE(m1 * m2 == result);
}

TEST_CASE("Mat2x2 multiplied by identity matrix remain the same", "[math][linalg][mat2x2]")
{
    Mat2x2 m{1.0f, 2.0f,
             3.0f, 4.0f};
    REQUIRE(m * Mat2x2::identity() == m);
}
