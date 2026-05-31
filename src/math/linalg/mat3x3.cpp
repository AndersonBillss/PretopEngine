#include "mat3x3.hpp"

bool Mat3x3::operator==(Mat3x3 &right) const
{
    return this->data[0] == right.data[0] &&
           this->data[1] == right.data[1] &&
           this->data[2] == right.data[2] &&
           this->data[3] == right.data[3] &&
           this->data[4] == right.data[4] &&
           this->data[5] == right.data[5] &&
           this->data[6] == right.data[6] &&
           this->data[7] == right.data[7] &&
           this->data[8] == right.data[8];
}

Vec3 Mat3x3::operator*(Vec3 &right) const
{
    return Vec3{
        this->data[0] * right.x + this->data[1] * right.y + this->data[2] * right.z,
        this->data[3] * right.x + this->data[4] * right.y + this->data[5] * right.z,
        this->data[6] * right.x + this->data[7] * right.y + this->data[8] * right.z,
    };
}

Mat3x3 Mat3x3::operator*(Mat3x3 &right) const
{
    return Mat3x3{
        this->data[0] * right.data[0] + this->data[1] * right.data[3] + this->data[2] * right.data[6],
        this->data[0] * right.data[1] + this->data[1] * right.data[4] + this->data[2] * right.data[7],
        this->data[0] * right.data[2] + this->data[1] * right.data[5] + this->data[2] * right.data[8],
        this->data[3] * right.data[0] + this->data[4] * right.data[3] + this->data[5] * right.data[6],
        this->data[3] * right.data[1] + this->data[4] * right.data[4] + this->data[5] * right.data[7],
        this->data[3] * right.data[2] + this->data[4] * right.data[5] + this->data[5] * right.data[8],
        this->data[6] * right.data[0] + this->data[7] * right.data[3] + this->data[8] * right.data[6],
        this->data[6] * right.data[1] + this->data[7] * right.data[4] + this->data[8] * right.data[7],
        this->data[6] * right.data[2] + this->data[7] * right.data[5] + this->data[8] * right.data[8],
    };
}

std::ostream &operator<<(std::ostream &os, const Mat3x3 &m)
{
    return os << "[[" << m(0, 0) << ", " << m(0, 1) << ", " << m(0, 2) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << ", " << m(1, 2) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << ", " << m(2, 2) << "]]";
}
