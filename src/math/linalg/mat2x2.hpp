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

    Vec2 operator *(Vec2 &right);
};