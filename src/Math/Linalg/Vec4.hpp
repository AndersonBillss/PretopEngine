#pragma once
#include <ostream>

struct Vec4
{
    float x;
    float y;
    float z;
    float w;

    float dot(const Vec4 &rhs) const;
    float length() const;
    void normalize();
    Vec4 normalized() const;
    bool nearlyEquals(const Vec4 &rhs, float epsilon = 1e-5f) const;
};

Vec4 operator+(const Vec4 &lhs, const Vec4 &rhs);
void operator+=(Vec4 &lhs, const Vec4 &rhs);
Vec4 operator-(const Vec4 &lhs, const Vec4 &rhs);
void operator-=(Vec4 &lhs, const Vec4 &rhs);
bool operator==(const Vec4 &lhs, const Vec4 &rhs);
std::ostream &operator<<(std::ostream &os, const Vec4 &v);
