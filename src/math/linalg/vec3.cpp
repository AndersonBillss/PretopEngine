#include <cmath>
#include "vec3.hpp"

Vec3 Vec3::operator+(const Vec3 &rhs) const
{
    return Vec3{
        this->x + rhs.x,
        this->y + rhs.y,
        this->z + rhs.z,
    };
}

void Vec3::operator+=(const Vec3 &rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
}

Vec3 Vec3::operator-(const Vec3 &rhs) const
{
    return Vec3{
        this->x - rhs.x,
        this->y - rhs.y,
        this->z - rhs.z,
    };
}

void Vec3::operator-=(const Vec3 &rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
}

bool Vec3::operator==(const Vec3 &rhs) const
{
    return this->x == rhs.x &&
           this->y == rhs.y &&
           this->z == rhs.z;
}

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

std::ostream &operator<<(std::ostream &os, const Vec3 &v)
{
    return os << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
}
