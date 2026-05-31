#pragma once
#include <ostream>

struct Vec3
{
    float x;
    float y;
    float z;

    float dot(const Vec3 &rhs) const;
    float length() const;
};

Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs);
void operator+=(Vec3 &lhs, const Vec3 &rhs);
Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs);
void operator-=(Vec3 &lhs, const Vec3 &rhs);
bool operator==(const Vec3 &lhs, const Vec3 &rhs);
std::ostream &operator<<(std::ostream &os, const Vec3 &v);
