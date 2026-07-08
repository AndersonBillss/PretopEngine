#pragma once
#include <ostream>
#include "Vec2.hpp"

namespace Pretop::Math
{
    struct Mat2x2
    {
        float Data[4];

        // Column-major storage: Data[col * 2 + row]
        inline float &operator()(int row, int col) { return Data[col * 2 + row]; }
        inline const float &operator()(int row, int col) const { return Data[col * 2 + row]; }

        inline float *begin() { return Data; }
        inline float *end() { return Data + 4; }
        inline const float *begin() const { return Data; }
        inline const float *end() const { return Data + 4; }

        static inline Mat2x2 Identity()
        {
            return Mat2x2{
                1, 0,
                0, 1};
        }
    };

    Mat2x2 Transpose(const Mat2x2 &mat);

    bool operator==(const Mat2x2 &left, const Mat2x2 &right);
    Vec2 operator*(const Mat2x2 &left, const Vec2 &right);
    Mat2x2 operator*(const Mat2x2 &left, const Mat2x2 &right);
    std::ostream &operator<<(std::ostream &os, const Mat2x2 &m);
} // namespace Pretop::Math
