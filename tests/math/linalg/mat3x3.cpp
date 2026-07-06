#include <catch2/catch_test_macros.hpp>
#include "../../../src/Math/Linalg/Mat3x3.hpp"

TEST_CASE("Mat3x3 Vec3 multiplication works", "[math][linalg][mat3x3]")
{
    Mat3x3 m{1.0f, 2.0f, 3.0f,
             4.0f, 1.0f, 5.0f,
             2.0f, 1.0f, 4.0f};
    Vec3 v{3.0f, 4.0f, 1.0f};
    Vec3 result{14.0f, 21.0f, 14.0f};
    REQUIRE(transpose(m) * v == result);
}

TEST_CASE("Mat3x3 Mat3x3 multiplication works", "[math][linalg][mat3x3]")
{
    Mat3x3 m1{1.0f, 2.0f, 3.0f,
              4.0f, 1.0f, 5.0f,
              2.0f, 1.0f, 4.0f};
    Mat3x3 m2{3.0f, 1.0f, 6.0f,
              2.0f, 5.0f, 9.0f,
              1.0f, 1.0f, 2.0f};
    Mat3x3 result{10.0f, 14.0f, 30.0f,
                  19.0f, 14.0f, 43.0f,
                  12.0f, 11.0f, 29.0f};
    REQUIRE(transpose(m1) * transpose(m2) == transpose(result));
}

TEST_CASE("Mat3x3 multiplied by identity matrix remain the same", "[math][linalg][mat3x3]")
{
    Mat3x3 m{1.0f, 2.0f, 3.0f,
             4.0f, 1.0f, 5.0f,
             2.0f, 1.0f, 4.0f};
    REQUIRE(m * Mat3x3::identity() == m);
}

TEST_CASE("Mat3x3 transpose works", "[math][linalg][mat3x3]")
{
    Mat3x3 m{1.0f, 2.0f, 3.0f,
             4.0f, 1.0f, 5.0f,
             2.0f, 1.0f, 4.0f};
    Mat3x3 m_t{1.0f, 4.0f, 2.0f,
               2.0f, 1.0f, 1.0f,
               3.0f, 5.0f, 4.0f};
    REQUIRE(transpose(m) == m_t);
}
