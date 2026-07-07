#pragma once

#include <ostream>
#include "Vec3.hpp"

struct Mat3x3
{
    float Data[9];

    // Column-major storage: Data[col * 3 + row]
    inline float &operator()(int row, int col) { return Data[col * 3 + row]; }
    inline const float &operator()(int row, int col) const { return Data[col * 3 + row]; }

    inline float *begin() { return Data; }
    inline float *end() { return Data + 9; }
    inline const float *begin() const { return Data; }
    inline const float *end() const { return Data + 9; }

    static inline Mat3x3 Identity()
    {
        return Mat3x3{1, 0, 0,
                      0, 1, 0,
                      0, 0, 1};
    }
};

Mat3x3 Transpose(const Mat3x3 &mat);

bool operator==(const Mat3x3 &left, const Mat3x3 &right);
Vec3 operator*(const Mat3x3 &left, const Vec3 &right);
Mat3x3 operator*(const Mat3x3 &left, const Mat3x3 &right);
std::ostream &operator<<(std::ostream &os, const Mat3x3 &m);
