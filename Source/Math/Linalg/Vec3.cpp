#include <cmath>
#include "Vec3.hpp"

float Vec3::dot(const Vec3 &rhs) const
{
    return this->x * rhs.x +
           this->y * rhs.y +
           this->z * rhs.z;
}

float Vec3::length() const
{
    return sqrt(
        this->x * this->x +
        this->y * this->y +
        this->z * this->z);
}

void Vec3::normalize()
{
    float len = this->length();
    this->x /= len;
    this->y /= len;
    this->z /= len;
}

Vec3 Vec3::normalized() const
{
    float len = this->length();
    return Vec3{this->x / len, this->y / len, this->z / len};
}

bool Vec3::nearlyEquals(const Vec3 &rhs, float epsilon) const
{
    return abs(this->x - rhs.x) <= epsilon &&
           abs(this->y - rhs.y) <= epsilon &&
           abs(this->z - rhs.z) <= epsilon;
}

Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs)
{
    return Vec3{
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
    };
}

void operator+=(Vec3 &lhs, const Vec3 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
}

Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs)
{
    return Vec3{
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
    };
}

void operator-=(Vec3 &lhs, const Vec3 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
}

bool operator==(const Vec3 &lhs, const Vec3 &rhs)
{
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.z == rhs.z;
}

std::ostream &operator<<(std::ostream &os, const Vec3 &v)
{
    return os << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
}
