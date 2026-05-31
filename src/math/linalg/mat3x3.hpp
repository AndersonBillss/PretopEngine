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

    bool operator==(Mat3x3 &right) const;
    Vec3 operator*(Vec3 &right) const;
    Mat3x3 operator*(Mat3x3 &right) const;
};

std::ostream &operator<<(std::ostream &os, const Mat3x3 &m);
