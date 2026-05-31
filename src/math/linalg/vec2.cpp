#include <cmath>
#include "vec2.hpp"

float Vec2::dot(const Vec2 &rhs) const
{
    return this->x * rhs.x + this->y * rhs.y;
}

float Vec2::length() const
{
    return sqrt(this->x * this->x + this->y * this->y);
}

Vec2 operator+(const Vec2 &lhs, const Vec2 &rhs)
{
    return Vec2{lhs.x + rhs.x, lhs.y + rhs.y};
}

void operator+=(Vec2 &lhs, const Vec2 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
}

Vec2 operator-(const Vec2 &lhs, const Vec2 &rhs)
{
    return Vec2{lhs.x - rhs.x, lhs.y - rhs.y};
}

void operator-=(Vec2 &lhs, const Vec2 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
}

bool operator==(const Vec2 &lhs, const Vec2 &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

std::ostream &operator<<(std::ostream &os, const Vec2 &v)
{
    return os << "Vec2(" << v.x << ", " << v.y << ")";
}
