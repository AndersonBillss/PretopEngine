#pragma once
#include "Linalg/Mat4x4.hpp"

namespace Pretop::Math
{
    struct Euler
    {
        float X;
        float Y;
        float Z;

        Mat4x4 ToMatrix();
    };
} // namespace Pretop::Math
