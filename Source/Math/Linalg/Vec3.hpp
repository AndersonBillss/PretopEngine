#pragma once
#include <ostream>

struct Vec3
{
    float X;
    float Y;
    float Z;

    float Dot(const Vec3 &rhs) const;
    float Length() const;
    void Normalize();
    Vec3 Normalized() const;
    bool NearlyEquals(const Vec3 &rhs, float epsilon = 1e-5f) const;
};

Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs);
void operator+=(Vec3 &lhs, const Vec3 &rhs);
Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs);
void operator-=(Vec3 &lhs, const Vec3 &rhs);
bool operator==(const Vec3 &lhs, const Vec3 &rhs);
std::ostream &operator<<(std::ostream &os, const Vec3 &v);
