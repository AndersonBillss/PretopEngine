#pragma once
#include <ostream>

struct Vec2
{
    float x;
    float y;

    float dot(const Vec2 &rhs) const;
    float length() const;
};

Vec2 operator+(const Vec2 &lhs, const Vec2 &rhs);
void operator+=(Vec2 &lhs, const Vec2 &rhs);
Vec2 operator-(const Vec2 &lhs, const Vec2 &rhs);
void operator-=(Vec2 &lhs, const Vec2 &rhs);
bool operator==(const Vec2 &lhs, const Vec2 &rhs);
std::ostream &operator<<(std::ostream &os, const Vec2 &v);
