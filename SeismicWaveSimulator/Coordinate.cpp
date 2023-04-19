#define _USE_MATH_DEFINES

#include <Coordinate.h>
#include <cmath>

Coordinate::Coordinate() {
    this->x = 0;
    this->y = 0;
}

Coordinate::Coordinate(double x, double y) {
    this->x = x;
    this->y = y;
}

Coordinate::Coordinate(const Coordinate& x) {
    this->x = x.x;
    this->y = x.y;
}

Coordinate Coordinate::operator+(Coordinate operand) {
    return Coordinate(this->x + operand.x, this->y + operand.y);
}

Coordinate Coordinate::operator-(const Coordinate operand) {
    return Coordinate(this->x - operand.x, this->y - operand.y);
}

Coordinate Coordinate::operator*(const double operand) {
    return Coordinate(this->x * operand, this->y * operand);
}

Coordinate Coordinate::operator+=(const Coordinate operand) {
    *this = *this + operand;
    return *this;
}

Coordinate Coordinate::operator-=(const Coordinate operand) {
    *this = *this - operand;
    return *this;
}

Coordinate Coordinate::operator*=(const double operand) {
    *this = *this * operand;
    return *this;
}

Point::Point() {
    this->direction = 0.0;
}

Point::Point(double x, double y, double dir) {
    this->position = Coord(x, y);
    this->direction = dir;
}

Point::Point(Coordinate& pos, double dir) {
    this->position = pos;
    this->direction = dir;
}

int Point::Move(double magnitude) {
    this->position += Coordinate(magnitude * cos(this->direction * M_PI), magnitude * sin(this->direction * M_PI));
}
