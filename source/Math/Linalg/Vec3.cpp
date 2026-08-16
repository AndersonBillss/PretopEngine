#include <cmath>
#include "Vec3.hpp"

namespace Pretop::Math
{
    float Vec3::Dot(const Vec3 &rhs) const
    {
        return this->X * rhs.X +
               this->Y * rhs.Y +
               this->Z * rhs.Z;
    }

    float Vec3::Length() const
    {
        return sqrt(
            this->X * this->X +
            this->Y * this->Y +
            this->Z * this->Z);
    }

    void Vec3::Normalize()
    {
        float len = this->Length();
        this->X /= len;
        this->Y /= len;
        this->Z /= len;
    }

    Vec3 Vec3::Normalized() const
    {
        float len = this->Length();
        return Vec3{this->X / len, this->Y / len, this->Z / len};
    }

    bool Vec3::NearlyEquals(const Vec3 &rhs, float epsilon) const
    {
        return abs(this->X - rhs.X) <= epsilon &&
               abs(this->Y - rhs.Y) <= epsilon &&
               abs(this->Z - rhs.Z) <= epsilon;
    }

    Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs)
    {
        return Vec3{
            lhs.X + rhs.X,
            lhs.Y + rhs.Y,
            lhs.Z + rhs.Z,
        };
    }

    void operator+=(Vec3 &lhs, const Vec3 &rhs)
    {
        lhs.X += rhs.X;
        lhs.Y += rhs.Y;
        lhs.Z += rhs.Z;
    }

    Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs)
    {
        return Vec3{
            lhs.X - rhs.X,
            lhs.Y - rhs.Y,
            lhs.Z - rhs.Z,
        };
    }

    void operator-=(Vec3 &lhs, const Vec3 &rhs)
    {
        lhs.X -= rhs.X;
        lhs.Y -= rhs.Y;
        lhs.Z -= rhs.Z;
    }

    bool operator==(const Vec3 &lhs, const Vec3 &rhs)
    {
        return lhs.X == rhs.X &&
               lhs.Y == rhs.Y &&
               lhs.Z == rhs.Z;
    }

    std::ostream &operator<<(std::ostream &os, const Vec3 &v)
    {
        return os << "Vec3(" << v.X << ", " << v.Y << ", " << v.Z << ")";
    }
} // namespace Pretop::Math
