#pragma once
#include <ostream>

struct Vec2 {
    float x;
    float y;

    Vec2 operator+(const Vec2& rhs) const;
    Vec2 operator-(const Vec2& rhs) const;
    bool operator==(const Vec2& rhs) const;
    float length() const;
};

std::ostream& operator<<(std::ostream& os, const Vec2& v);
