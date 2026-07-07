#include <cmath>
#include "Vec2.hpp"

float Vec2::Dot(const Vec2 &rhs) const
{
    return this->X * rhs.X + this->Y * rhs.Y;
}

float Vec2::Length() const
{
    return sqrt(this->X * this->X + this->Y * this->Y);
}

void Vec2::Normalize()
{
    float len = this->Length();
    this->X /= len;
    this->Y /= len;
}

Vec2 Vec2::Normalized() const
{
    float len = this->Length();
    return Vec2{this->X / len, this->Y / len};
}

bool Vec2::NearlyEquals(const Vec2 &rhs, float epsilon) const
{
    return abs(this->X - rhs.X) <= epsilon &&
           abs(this->Y - rhs.Y) <= epsilon;
}

Vec2 operator+(const Vec2 &lhs, const Vec2 &rhs)
{
    return Vec2{lhs.X + rhs.X, lhs.Y + rhs.Y};
}

void operator+=(Vec2 &lhs, const Vec2 &rhs)
{
    lhs.X += rhs.X;
    lhs.Y += rhs.Y;
}

Vec2 operator-(const Vec2 &lhs, const Vec2 &rhs)
{
    return Vec2{lhs.X - rhs.X, lhs.Y - rhs.Y};
}

void operator-=(Vec2 &lhs, const Vec2 &rhs)
{
    lhs.X -= rhs.X;
    lhs.Y -= rhs.Y;
}

bool operator==(const Vec2 &lhs, const Vec2 &rhs)
{
    return lhs.X == rhs.X && lhs.Y == rhs.Y;
}

std::ostream &operator<<(std::ostream &os, const Vec2 &v)
{
    return os << "Vec2(" << v.X << ", " << v.Y << ")";
}
