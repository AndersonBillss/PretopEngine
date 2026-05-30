#include <cmath>
#include "vec2.hpp"

Vec2 Vec2::operator+(const Vec2 &rhs) const
{
    return Vec2{this->x + rhs.x, this->y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2 &rhs) const
{
    return Vec2{this->x - rhs.x, this->y - rhs.y};
}

bool Vec2::operator==(const Vec2 &rhs) const
{
    return this->x == rhs.x && this->y == rhs.y;
}

float Vec2::length() const
{
    return sqrt(this->x * this->x + this->y * this->y);
}

std::ostream &operator<<(std::ostream &os, const Vec2 &v)
{
    return os << "Vec2(" << v.x << ", " << v.y << ")";
}
