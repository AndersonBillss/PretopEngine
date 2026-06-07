#pragma once
#include "linalg/mat4x4.hpp"

struct Euler {
    float x;
    float y;
    float z;

    Mat4x4 toMatrix();
};
