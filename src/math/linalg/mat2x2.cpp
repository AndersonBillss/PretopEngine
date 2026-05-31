#include "mat2x2.hpp"

bool Mat2x2::operator==(Mat2x2 &right) const
{
    return this->data[0] == right.data[0] &&
           this->data[1] == right.data[1] &&
           this->data[2] == right.data[2] &&
           this->data[3] == right.data[3];
}

Vec2 Mat2x2::operator*(Vec2 &right) const
{
    return Vec2{
        this->data[0] * right.x + this->data[1] * right.y,
        this->data[2] * right.x + this->data[3] * right.y,
    };
}

Mat2x2 Mat2x2::operator*(Mat2x2 &right) const
{
    return Mat2x2{
        this->data[0] * right.data[0] + this->data[1] * right.data[2],
        this->data[0] * right.data[1] + this->data[1] * right.data[3],
        this->data[2] * right.data[0] + this->data[3] * right.data[2],
        this->data[2] * right.data[1] + this->data[3] * right.data[3],
    };
}

std::ostream &operator<<(std::ostream &os, const Mat2x2 &m)
{
    return os << "[[" << m(0, 0) << ", " << m(0, 1) << "],\n"
              << "[" << m(1, 0) << ", " << m(1, 1) << "]]";
}
