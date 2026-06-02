#include "mat2x2.hpp"

Mat2x2 transpose(Mat2x2 &mat)
{
    return Mat2x2{mat(0, 0), mat(1, 0),
                  mat(0, 1), mat(1, 1)};
}

bool operator==(const Mat2x2 &left, const Mat2x2 &right)
{
    return left.data[0] == right.data[0] &&
           left.data[1] == right.data[1] &&
           left.data[2] == right.data[2] &&
           left.data[3] == right.data[3];
}

Vec2 operator*(const Mat2x2 &left, const Vec2 &right)
{
    return Vec2{
        left.data[0] * right.x + left.data[1] * right.y,
        left.data[2] * right.x + left.data[3] * right.y,
    };
}

Mat2x2 operator*(const Mat2x2 &left, const Mat2x2 &right)
{
    return Mat2x2{
        left.data[0] * right.data[0] + left.data[1] * right.data[2],
        left.data[0] * right.data[1] + left.data[1] * right.data[3],
        left.data[2] * right.data[0] + left.data[3] * right.data[2],
        left.data[2] * right.data[1] + left.data[3] * right.data[3],
    };
}

std::ostream &operator<<(std::ostream &os, const Mat2x2 &m)
{
    return os << "[[" << m(0, 0) << ", " << m(0, 1) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << "]]";
}
