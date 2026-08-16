#include "Mat4x4.hpp"
#include <cmath>

namespace Pretop::Math
{
    Mat4x4 Transpose(const Mat4x4 &mat)
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

    Mat4x4 Mat4x4::Perspective(float near, float far, float fovY, float aspect)
    {
        float f = 1.0f / tan(fovY * 0.5f);
        return Mat4x4{
            f / aspect, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, far / (near - far), -1.0f,
            0.0f, 0.0f, (near * far) / (near - far), 0.0f};
    }

    Mat4x4 Mat4x4::Transform(float x, float y, float z)
    {
        return Mat4x4{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x, y, z, 1.0f};
    }

    Mat4x4 Mat4x4::Scale(float factor)
    {
        return Mat4x4{
            factor, 0.0f, 0.0f, 0.0f,
            0.0f, factor, 0.0f, 0.0f,
            0.0f, 0.0f, factor, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
    }

    Mat4x4 Mat4x4::Scale(float x, float y, float z)
    {
        return Mat4x4{
            x, 0.0f, 0.0f, 0.0f,
            0.0f, y, 0.0f, 0.0f,
            0.0f, 0.0f, z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
    }

    bool operator==(const Mat4x4 &left, const Mat4x4 &right)
    {
        for (int i = 0; i < 16; ++i)
        {
            if (left.Data[i] != right.Data[i])
            {
                return false;
            }
        }
        return true;
    }

    Vec4 operator*(const Mat4x4 &left, const Vec4 &right)
    {
        return Vec4{
            left(0, 0) * right.X + left(0, 1) * right.Y + left(0, 2) * right.Z + left(0, 3) * right.W,
            left(1, 0) * right.X + left(1, 1) * right.Y + left(1, 2) * right.Z + left(1, 3) * right.W,
            left(2, 0) * right.X + left(2, 1) * right.Y + left(2, 2) * right.Z + left(2, 3) * right.W,
            left(3, 0) * right.X + left(3, 1) * right.Y + left(3, 2) * right.Z + left(3, 3) * right.W,
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
} // namespace Pretop::Math
