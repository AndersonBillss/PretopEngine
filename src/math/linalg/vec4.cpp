#include <cmath>
#include "vec4.hpp"

float Vec4::dot(const Vec4 &rhs) const
{
    return this->x * rhs.x +
           this->y * rhs.y +
           this->z * rhs.z +
           this->w * rhs.w;
}

float Vec4::length() const
{
    return sqrt(
        this->x * this->x +
        this->y * this->y +
        this->z * this->z +
        this->w * this->w);
}

void Vec4::normalize()
{
    float len = this->length();
    this->x /= len;
    this->y /= len;
    this->z /= len;
    this->w /= len;
}

Vec4 Vec4::normalized() const
{
    float len = this->length();
    return Vec4{this->x / len, this->y / len, this->z / len, this->w / len};
}

Vec4 operator+(const Vec4 &lhs, const Vec4 &rhs)
{
    return Vec4{
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w,
    };
}

void operator+=(Vec4 &lhs, const Vec4 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
}

Vec4 operator-(const Vec4 &lhs, const Vec4 &rhs)
{
    return Vec4{
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w,
    };
}

void operator-=(Vec4 &lhs, const Vec4 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
}

bool operator==(const Vec4 &lhs, const Vec4 &rhs)
{
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.z == rhs.z &&
           lhs.w == rhs.w;
}

std::ostream &operator<<(std::ostream &os, const Vec4 &v)
{
    return os << "Vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}
