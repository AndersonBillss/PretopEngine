#pragma once

#include <ostream>
#include "Vec4.hpp"

struct Mat4x4
{
    float Data[16];

    inline float &operator()(int row, int col) { return Data[col * 4 + row]; }
    inline const float &operator()(int row, int col) const { return Data[col * 4 + row]; }

    inline float *begin() { return Data; }
    inline float *end() { return Data + 16; }
    inline const float *begin() const { return Data; }
    inline const float *end() const { return Data + 16; }

    static inline Mat4x4 Identity()
    {
        return Mat4x4{1, 0, 0, 0,
                      0, 1, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1};
    }
    static Mat4x4 Perspective(float near, float far, float fovY, float aspect);
    static Mat4x4 Transform(float x, float y, float z);
    static Mat4x4 Scale(float factor);
    static Mat4x4 Scale(float x, float y, float z);
};

Mat4x4 Transpose(const Mat4x4 &mat);

bool operator==(const Mat4x4 &left, const Mat4x4 &right);
Vec4 operator*(const Mat4x4 &left, const Vec4 &right);
Mat4x4 operator*(const Mat4x4 &left, const Mat4x4 &right);
std::ostream &operator<<(std::ostream &os, const Mat4x4 &m);
