#include "euler.hpp"

Mat4x4 Euler::toMatrix()
{
    float cx = cos(this->roll);
    float sx = sin(this->roll);
    float cy = cos(this->pitch);
    float sy = sin(this->pitch);
    float cz = cos(this->yaw);
    float sz = sin(this->yaw);
    return Mat4x4{
        cz * cy, sz * cy, -sy, 0.0f,
        cz * sy * sx - sz * cx, sz * sy * sx + cz * cx, cy * sx, 0.0f,
        cz * sy * cx + sz * sx, sz * sy * cx - cz * sx, cy * cx, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
}