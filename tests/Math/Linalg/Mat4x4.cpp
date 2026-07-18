#include <catch2/catch_test_macros.hpp>
#include "../../../Source/Math/Linalg/Mat4x4.hpp"

using namespace Pretop::Math;

TEST_CASE("Mat4x4 Vec4 multiplication works", "[Math][Linalg][Mat4x4]")
{
    Mat4x4 m{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f, 7.0f};

    Vec4 v{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 result{
        30.0f,
        70.0f,
        29.0f,
        60.0f};

    REQUIRE(Transpose(m) * v == result);
}

TEST_CASE("Mat4x4 Mat4x4 multiplication works", "[Math][Linalg][Mat4x4]")
{
    Mat4x4 m1{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f, 7.0f};

    Mat4x4 m2{
        1.0f, 0.0f, 2.0f, 1.0f,
        3.0f, 1.0f, 0.0f, 2.0f,
        4.0f, 2.0f, 1.0f, 0.0f,
        0.0f, 3.0f, 2.0f, 1.0f};

    Mat4x4 result{
        19.0f, 20.0f, 13.0f, 9.0f,
        51.0f, 44.0f, 33.0f, 25.0f,
        20.0f, 14.0f, 26.0f, 14.0f,
        43.0f, 38.0f, 28.0f, 21.0f};

    REQUIRE(Transpose(m1) * Transpose(m2) == Transpose(result));
}

TEST_CASE("Mat4x4 multiplied by identity matrix remain the same", "[Math][Linalg][Mat4x4]")
{
    Mat4x4 m{
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f, 7.0f};

    REQUIRE(m * Mat4x4::Identity() == m);
}

TEST_CASE("Mat4x4 transpose works", "[Math][Linalg][Mat4x4]")
{
    Mat4x4 m{
        1.0f, 2.0f, 3.0f, 10.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f, 7.0f};
    Mat4x4 mT{
        1.0f, 5.0f, 9.0f, 4.0f,
        2.0f, 6.0f, 1.0f, 5.0f,
        3.0f, 7.0f, 2.0f, 6.0f,
        10.0f, 8.0f, 3.0f, 7.0f};
    REQUIRE(Transpose(m) == mT);
}
