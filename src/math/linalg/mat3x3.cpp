#include "mat3x3.hpp"

bool operator==(const Mat3x3 &left, const Mat3x3 &right)
{
    return left.data[0] == right.data[0] &&
           left.data[1] == right.data[1] &&
           left.data[2] == right.data[2] &&
           left.data[3] == right.data[3] &&
           left.data[4] == right.data[4] &&
           left.data[5] == right.data[5] &&
           left.data[6] == right.data[6] &&
           left.data[7] == right.data[7] &&
           left.data[8] == right.data[8];
}

Vec3 operator*(const Mat3x3 &left, const Vec3 &right)
{
    return Vec3{
        left.data[0] * right.x + left.data[1] * right.y + left.data[2] * right.z,
        left.data[3] * right.x + left.data[4] * right.y + left.data[5] * right.z,
        left.data[6] * right.x + left.data[7] * right.y + left.data[8] * right.z,
    };
}

Mat3x3 operator*(const Mat3x3 &left, const Mat3x3 &right)
{
    return Mat3x3{
        left.data[0] * right.data[0] + left.data[1] * right.data[3] + left.data[2] * right.data[6],
        left.data[0] * right.data[1] + left.data[1] * right.data[4] + left.data[2] * right.data[7],
        left.data[0] * right.data[2] + left.data[1] * right.data[5] + left.data[2] * right.data[8],
        left.data[3] * right.data[0] + left.data[4] * right.data[3] + left.data[5] * right.data[6],
        left.data[3] * right.data[1] + left.data[4] * right.data[4] + left.data[5] * right.data[7],
        left.data[3] * right.data[2] + left.data[4] * right.data[5] + left.data[5] * right.data[8],
        left.data[6] * right.data[0] + left.data[7] * right.data[3] + left.data[8] * right.data[6],
        left.data[6] * right.data[1] + left.data[7] * right.data[4] + left.data[8] * right.data[7],
        left.data[6] * right.data[2] + left.data[7] * right.data[5] + left.data[8] * right.data[8],
    };
}

std::ostream &operator<<(std::ostream &os, const Mat3x3 &m)
{
    return os << "[[" << m(0, 0) << ", " << m(0, 1) << ", " << m(0, 2) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << ", " << m(1, 2) << "],\n"
              << "[" << m(2, 0) << ", " << m(2, 1) << ", " << m(2, 2) << "]]";
}
