#pragma once
#include <ostream>

struct Vec2
{
    float X;
    float Y;

    float Dot(const Vec2 &rhs) const;
    float Length() const;
    void Normalize();
    Vec2 Normalized() const;
    bool NearlyEquals(const Vec2 &rhs, float epsilon = 1e-5f) const;
};

Vec2 operator+(const Vec2 &lhs, const Vec2 &rhs);
void operator+=(Vec2 &lhs, const Vec2 &rhs);
Vec2 operator-(const Vec2 &lhs, const Vec2 &rhs);
void operator-=(Vec2 &lhs, const Vec2 &rhs);
bool operator==(const Vec2 &lhs, const Vec2 &rhs);
std::ostream &operator<<(std::ostream &os, const Vec2 &v);
