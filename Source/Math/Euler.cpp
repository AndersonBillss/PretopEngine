#include <cmath>
#include "Euler.hpp"

Mat4x4 Euler::ToMatrix()
{
    float x = this->X;
    float y = this->Y;
    float z = this->Z;

    float cx = cos(x);
    float sx = sin(x);
    float cy = cos(y);
    float sy = sin(y);
    float cz = cos(z);
    float sz = sin(z);

    return Mat4x4{
        cz * cy, sz * cy, -sy, 0.0f,
        cz * sy * sx - sz * cx, sz * sy * sx + cz * cx, cy * sx, 0.0f,
        cz * sy * cx + sz * sx, sz * sy * cx - cz * sx, cy * cx, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}