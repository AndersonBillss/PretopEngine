#include <cmath>
#include "Vec4.hpp"

namespace Pretop::Math
{
    float Vec4::Dot(const Vec4 &rhs) const
    {
        return this->X * rhs.X +
               this->Y * rhs.Y +
               this->Z * rhs.Z +
               this->W * rhs.W;
    }

    float Vec4::Length() const
    {
        return sqrt(
            this->X * this->X +
            this->Y * this->Y +
            this->Z * this->Z +
            this->W * this->W);
    }

    void Vec4::Normalize()
    {
        float len = this->Length();
        this->X /= len;
        this->Y /= len;
        this->Z /= len;
        this->W /= len;
    }

    Vec4 Vec4::Normalized() const
    {
        float len = this->Length();
        return Vec4{this->X / len, this->Y / len, this->Z / len, this->W / len};
    }

    bool Vec4::NearlyEquals(const Vec4 &rhs, float epsilon) const
    {
        return abs(this->X - rhs.X) <= epsilon &&
               abs(this->Y - rhs.Y) <= epsilon &&
               abs(this->Z - rhs.Z) <= epsilon &&
               abs(this->W - rhs.W) <= epsilon;
    }

    Vec4 operator+(const Vec4 &lhs, const Vec4 &rhs)
    {
        return Vec4{
            lhs.X + rhs.X,
            lhs.Y + rhs.Y,
            lhs.Z + rhs.Z,
            lhs.W + rhs.W,
        };
    }

    void operator+=(Vec4 &lhs, const Vec4 &rhs)
    {
        lhs.X += rhs.X;
        lhs.Y += rhs.Y;
        lhs.Z += rhs.Z;
        lhs.W += rhs.W;
    }

    Vec4 operator-(const Vec4 &lhs, const Vec4 &rhs)
    {
        return Vec4{
            lhs.X - rhs.X,
            lhs.Y - rhs.Y,
            lhs.Z - rhs.Z,
            lhs.W - rhs.W,
        };
    }

    void operator-=(Vec4 &lhs, const Vec4 &rhs)
    {
        lhs.X -= rhs.X;
        lhs.Y -= rhs.Y;
        lhs.Z -= rhs.Z;
        lhs.W -= rhs.W;
    }

    bool operator==(const Vec4 &lhs, const Vec4 &rhs)
    {
        return lhs.X == rhs.X &&
               lhs.Y == rhs.Y &&
               lhs.Z == rhs.Z &&
               lhs.W == rhs.W;
    }

    std::ostream &operator<<(std::ostream &os, const Vec4 &v)
    {
        return os << "Vec4(" << v.X << ", " << v.Y << ", " << v.Z << ", " << v.W << ")";
    }
} // namespace Pretop::Math
