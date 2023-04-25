#define _USE_MATH_DEFINES

#include <Coordinate.h>
#include <cmath>
#include <string>
#include <Log.h>
using namespace std;

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

std::string Coordinate::to_str() {
	return "(" + to_string(this->x) + ", " + to_string(this->y) + ")";
}

Point::Point() {
	this->position.x = 0;
	this->position.y = 0;
	this->direction = 0;
	this->previous_layer = nullptr;
}

Point::Point(double x, double y, double dir) {
    this->position = Coord(x, y);
    this->direction = dir;
	this->previous_layer = nullptr;
}

Point::Point(Coordinate& pos, double dir) {
	this->position = pos;
	this->direction = dir;
	this->previous_layer = nullptr;
}

int Point::Move(double magnitude) {
    this->position += Coordinate(magnitude * cos(this->direction * M_PI), magnitude * sin(this->direction * M_PI));
	return 0;
}

bool Point::LayerChanged(void* layer) {
	void* p = this->previous_layer;
	this->previous_layer = layer;
	return (layer != p && p != nullptr);
}

void* Point::GetPreviousLayer() {
	return this->previous_layer;
}
