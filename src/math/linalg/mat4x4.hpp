#pragma once

#include <ostream>
#include "vec4.hpp"

struct Mat4x4
{
    float data[16];

    inline float &operator()(int row, int col) { return data[col * 4 + row]; }
    inline const float &operator()(int row, int col) const { return data[col * 4 + row]; }

    inline float *begin() { return data; }
    inline float *end() { return data + 16; }
    inline const float *begin() const { return data; }
    inline const float *end() const { return data + 16; }

    static inline Mat4x4 identity()
    {
        return Mat4x4{1, 0, 0, 0,
                      0, 1, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1};
    }
    static Mat4x4 perspective(float near, float far, float fovY, float aspect);
    static Mat4x4 transform(float x, float y, float z);
};

Mat4x4 transpose(const Mat4x4 &mat);

bool operator==(const Mat4x4 &left, const Mat4x4 &right);
Vec4 operator*(const Mat4x4 &left, const Vec4 &right);
Mat4x4 operator*(const Mat4x4 &left, const Mat4x4 &right);
std::ostream &operator<<(std::ostream &os, const Mat4x4 &m);