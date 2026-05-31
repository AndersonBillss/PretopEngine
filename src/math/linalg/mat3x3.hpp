#pragma once
#include "vec3.hpp"

struct Mat3x3
{
    float data[9];

    inline float &operator()(int row, int col) { return data[row * 3 + col]; }
    inline const float &operator()(int row, int col) const { return data[row * 3 + col]; }
    inline float *begin() { return data; }
    inline float *end() { return data + 9; }
    inline const float *begin() const { return data; }
    inline const float *end() const { return data + 9; }
    static inline Mat3x3 identity() { return Mat3x3{1, 0, 0, 0, 1, 0, 0, 0, 1}; }
};

bool operator==(const Mat3x3 &left, const Mat3x3 &right);
Vec3 operator*(const Mat3x3 &left, const Vec3 &right);
Mat3x3 operator*(const Mat3x3 &left, const Mat3x3 &right);
std::ostream &operator<<(std::ostream &os, const Mat3x3 &m);
