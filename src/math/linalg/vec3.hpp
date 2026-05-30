
#pragma once
#include <ostream>

struct Vec3 {
    float x;
    float y;
    float z;

    Vec3 operator+(const Vec3& rhs) const;
    void operator+=(const Vec3& rhs);
    Vec3 operator-(const Vec3& rhs) const;
    void operator-=(const Vec3& rhs);
    bool operator==(const Vec3& rhs) const;
    float dot(const Vec3& rhs) const;
    float length() const;
};

std::ostream& operator<<(std::ostream& os, const Vec3& v);
