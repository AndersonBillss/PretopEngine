#include "mat2x2.hpp"

Mat2x2 transpose(const Mat2x2 &mat)
{
    Mat2x2 result{};

    for (int row = 0; row < 2; ++row)
    {
        for (int col = 0; col < 2; ++col)
        {
            result(row, col) = mat(col, row);
        }
    }

    return result;
}

bool operator==(const Mat2x2 &left, const Mat2x2 &right)
{
    for (int i = 0; i < 4; ++i)
    {
        if (left.data[i] != right.data[i])
        {
            return false;
        }
    }
    return true;
}

Vec2 operator*(const Mat2x2 &left, const Vec2 &right)
{
    return Vec2{
        left(0, 0) * right.x + left(0, 1) * right.y,
        left(1, 0) * right.x + left(1, 1) * right.y,
    };
}

Mat2x2 operator*(const Mat2x2 &left, const Mat2x2 &right)
{
    Mat2x2 result{};

    for (int row = 0; row < 2; ++row)
    {
        for (int col = 0; col < 2; ++col)
        {
            float sum = 0.0f;
            for (int k = 0; k < 2; ++k)
            {
                sum += left(row, k) * right(k, col);
            }

            result(row, col) = sum;
        }
    }

    return result;
}

std::ostream &operator<<(std::ostream &os, const Mat2x2 &m)
{
    return os << "[[" << m(0, 0) << ", " << m(0, 1) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << "]]";
}