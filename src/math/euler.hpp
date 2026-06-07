#pragma once
#include "linalg/mat4x4.hpp"

struct Euler {
    float roll;
    float pitch;
    float yaw;

    Mat4x4 toMatrix();
};
