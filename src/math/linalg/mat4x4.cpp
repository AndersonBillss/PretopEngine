#include "mat4x4.hpp"

Mat4x4 transpose(Mat4x4 &mat)
{
    return Mat4x4{mat(0, 0), mat(1, 0), mat(2, 0), mat(3, 0),
                  mat(0, 1), mat(1, 1), mat(2, 1), mat(3, 1),
                  mat(0, 2), mat(1, 2), mat(2, 2), mat(3, 2),
                  mat(0, 3), mat(1, 3), mat(2, 3), mat(3, 3)};
}

bool operator==(const Mat4x4 &left, const Mat4x4 &right)
{
    return left.data[0] == right.data[0] &&
           left.data[1] == right.data[1] &&
           left.data[2] == right.data[2] &&
           left.data[3] == right.data[3] &&
           left.data[4] == right.data[4] &&
           left.data[5] == right.data[5] &&
           left.data[6] == right.data[6] &&
           left.data[7] == right.data[7] &&
           left.data[8] == right.data[8] &&
           left.data[9] == right.data[9] &&
           left.data[10] == right.data[10] &&
           left.data[11] == right.data[11] &&
           left.data[12] == right.data[12] &&
           left.data[13] == right.data[13] &&
           left.data[14] == right.data[14] &&
           left.data[15] == right.data[15];
}

Vec4 operator*(const Mat4x4 &left, const Vec4 &right)
{
    return Vec4{
        left.data[0] * right.x + left.data[1] * right.y + left.data[2] * right.z + left.data[3] * right.w,
        left.data[4] * right.x + left.data[5] * right.y + left.data[6] * right.z + left.data[7] * right.w,
        left.data[8] * right.x + left.data[9] * right.y + left.data[10] * right.z + left.data[11] * right.w,
        left.data[12] * right.x + left.data[13] * right.y + left.data[14] * right.z + left.data[15] * right.w,
    };
}

Mat4x4 operator*(const Mat4x4 &left, const Mat4x4 &right)
{
    return Mat4x4{
        left.data[0] * right.data[0] + left.data[1] * right.data[4] + left.data[2] * right.data[8] + left.data[3] * right.data[12],
        left.data[0] * right.data[1] + left.data[1] * right.data[5] + left.data[2] * right.data[9] + left.data[3] * right.data[13],
        left.data[0] * right.data[2] + left.data[1] * right.data[6] + left.data[2] * right.data[10] + left.data[3] * right.data[14],
        left.data[0] * right.data[3] + left.data[1] * right.data[7] + left.data[2] * right.data[11] + left.data[3] * right.data[15],
        left.data[4] * right.data[0] + left.data[5] * right.data[4] + left.data[6] * right.data[8] + left.data[7] * right.data[12],
        left.data[4] * right.data[1] + left.data[5] * right.data[5] + left.data[6] * right.data[9] + left.data[7] * right.data[13],
        left.data[4] * right.data[2] + left.data[5] * right.data[6] + left.data[6] * right.data[10] + left.data[7] * right.data[14],
        left.data[4] * right.data[3] + left.data[5] * right.data[7] + left.data[6] * right.data[11] + left.data[7] * right.data[15],
        left.data[8] * right.data[0] + left.data[9] * right.data[4] + left.data[10] * right.data[8] + left.data[11] * right.data[12],
        left.data[8] * right.data[1] + left.data[9] * right.data[5] + left.data[10] * right.data[9] + left.data[11] * right.data[13],
        left.data[8] * right.data[2] + left.data[9] * right.data[6] + left.data[10] * right.data[10] + left.data[11] * right.data[14],
        left.data[8] * right.data[3] + left.data[9] * right.data[7] + left.data[10] * right.data[11] + left.data[11] * right.data[15],
        left.data[12] * right.data[0] + left.data[13] * right.data[4] + left.data[14] * right.data[8] + left.data[15] * right.data[12],
        left.data[12] * right.data[1] + left.data[13] * right.data[5] + left.data[14] * right.data[9] + left.data[15] * right.data[13],
        left.data[12] * right.data[2] + left.data[13] * right.data[6] + left.data[14] * right.data[10] + left.data[15] * right.data[14],
        left.data[12] * right.data[3] + left.data[13] * right.data[7] + left.data[14] * right.data[11] + left.data[15] * right.data[15],
    };
}

std::ostream &operator<<(std::ostream &os, const Mat4x4 &m)
{
    return os << "[[" << m(0, 0) << ", " << m(0, 1) << ", " << m(0, 2) << ", " << m(0, 3) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << ", " << m(1, 2) << ", " << m(1, 3) << "],\n"
              << "[" << m(2, 0) << ", " << m(2, 1) << ", " << m(2, 2) << ", " << m(2, 3) << "],\n"
              << "[" << m(3, 0) << ", " << m(3, 1) << ", " << m(3, 2) << ", " << m(3, 3) << "]]";
}
