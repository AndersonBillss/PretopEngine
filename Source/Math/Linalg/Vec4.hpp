#pragma once
#include <ostream>

namespace Pretop::Math
{
    struct Vec4
    {
        float X;
        float Y;
        float Z;
        float W;

        float Dot(const Vec4 &rhs) const;
        float Length() const;
        void Normalize();
        Vec4 Normalized() const;
        bool NearlyEquals(const Vec4 &rhs, float epsilon = 1e-5f) const;
    };

    Vec4 operator+(const Vec4 &lhs, const Vec4 &rhs);
    void operator+=(Vec4 &lhs, const Vec4 &rhs);
    Vec4 operator-(const Vec4 &lhs, const Vec4 &rhs);
    void operator-=(Vec4 &lhs, const Vec4 &rhs);
    bool operator==(const Vec4 &lhs, const Vec4 &rhs);
    std::ostream &operator<<(std::ostream &os, const Vec4 &v);
} // namespace Pretop::Math
