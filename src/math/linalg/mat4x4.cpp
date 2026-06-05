#include "mat4x4.hpp"

Mat4x4 transpose(const Mat4x4 &mat)
{
    Mat4x4 result{};

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result(row, col) = mat(col, row);
        }
    }

    return result;
}

Mat4x4 perspective(float near, float far, float fovY, float aspect)
{
    float f = 1.0f / tan(fovY * 0.5f);
    return Mat4x4{
        f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, far / (near - far), -1.0f,
        0.0f, 0.0f, (near * far) / (near - far), 0.0f};
}

bool operator==(const Mat4x4 &left, const Mat4x4 &right)
{
    for (int i = 0; i < 16; ++i)
    {
        if (left.data[i] != right.data[i])
        {
            return false;
        }
    }
    return true;
}

Vec4 operator*(const Mat4x4 &left, const Vec4 &right)
{
    return Vec4{
        left(0, 0) * right.x + left(0, 1) * right.y + left(0, 2) * right.z + left(0, 3) * right.w,
        left(1, 0) * right.x + left(1, 1) * right.y + left(1, 2) * right.z + left(1, 3) * right.w,
        left(2, 0) * right.x + left(2, 1) * right.y + left(2, 2) * right.z + left(2, 3) * right.w,
        left(3, 0) * right.x + left(3, 1) * right.y + left(3, 2) * right.z + left(3, 3) * right.w,
    };
}

Mat4x4 operator*(const Mat4x4 &left, const Mat4x4 &right)
{
    Mat4x4 result{};

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                sum += left(row, k) * right(k, col);
            }
            result(row, col) = sum;
        }
    }

    return result;
}

std::ostream &operator<<(std::ostream &os, const Mat4x4 &m)
{
    return os << "[[" << m(0, 0) << ", " << m(0, 1) << ", " << m(0, 2) << ", " << m(0, 3) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << ", " << m(1, 2) << ", " << m(1, 3) << "],\n"
              << "[" << m(2, 0) << ", " << m(2, 1) << ", " << m(2, 2) << ", " << m(2, 3) << "],\n"
              << "[" << m(3, 0) << ", " << m(3, 1) << ", " << m(3, 2) << ", " << m(3, 3) << "]]";
}
