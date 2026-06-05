#pragma once
#include "vec2.hpp"

struct Mat2x2
{
    float data[4];

    inline float &operator()(int row, int col) { return data[row * 2 + col]; }
    inline const float &operator()(int row, int col) const { return data[row * 2 + col]; }
    inline float *begin() { return data; }
    inline float *end() { return data + 4; }
    inline const float *begin() const { return data; }
    inline const float *end() const { return data + 4; }
    static inline Mat2x2 identity() { return Mat2x2{1, 0, 0, 1}; }
};

Mat2x2 transpose(const Mat2x2 &mat);

bool operator==(const Mat2x2 &left, const Mat2x2 &right);
Vec2 operator*(const Mat2x2 &left, const Vec2 &right);
Mat2x2 operator*(const Mat2x2 &left, const Mat2x2 &right);
std::ostream &operator<<(std::ostream &os, const Mat2x2 &m);
