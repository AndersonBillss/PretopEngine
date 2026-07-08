#include "Mat3x3.hpp"

namespace Pretop::Math
{
    Mat3x3 Transpose(const Mat3x3 &mat)
    {
        Mat3x3 result{};

        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 3; ++col)
            {
                result(row, col) = mat(col, row);
            }
        }

        return result;
    }

    bool operator==(const Mat3x3 &left, const Mat3x3 &right)
    {
        for (int i = 0; i < 9; ++i)
        {
            if (left.Data[i] != right.Data[i])
            {
                return false;
            }
        }
        return true;
    }

    Vec3 operator*(const Mat3x3 &left, const Vec3 &right)
    {
        return Vec3{
            left(0, 0) * right.X + left(0, 1) * right.Y + left(0, 2) * right.Z,
            left(1, 0) * right.X + left(1, 1) * right.Y + left(1, 2) * right.Z,
            left(2, 0) * right.X + left(2, 1) * right.Y + left(2, 2) * right.Z,
        };
    }

    Mat3x3 operator*(const Mat3x3 &left, const Mat3x3 &right)
    {
        Mat3x3 result{};

        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 3; ++col)
            {
                float sum = 0.0f;
                for (int k = 0; k < 3; ++k)
                {
                    sum += left(row, k) * right(k, col);
                }
                result(row, col) = sum;
            }
        }

        return result;
    }

    std::ostream &operator<<(std::ostream &os, const Mat3x3 &m)
    {
        return os << "[[" << m(0, 0) << ", " << m(0, 1) << ", " << m(0, 2) << "],\n"
                  << "[" << m(1, 0) << ", " << m(1, 1) << ", " << m(1, 2) << "],\n"
                  << "[" << m(2, 0) << ", " << m(2, 1) << ", " << m(2, 2) << "]]";
    }
} // namespace Pretop::Math
