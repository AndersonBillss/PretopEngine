#include "mat2x2.hpp"

Vec2 Mat2x2::operator*(Vec2 &right)
{
    return Vec2{
        this->data[0] * right.x + this->data[1] * right.y,
        this->data[2] * right.x + this->data[3] * right.y,
    };
}