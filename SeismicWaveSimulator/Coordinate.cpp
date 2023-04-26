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
	this->intensity = 1.0;
	this->temp_layer = false;
	this->previous_layer = nullptr;
}

Point::Point(const Point& point) {
	this->position = point.position;
	this->direction = point.direction;
	this->intensity = point.intensity;
	this->temp_layer = point.temp_layer;
	this->previous_layer = point.previous_layer;
}

Point::Point(double x, double y, double dir) {
    this->position = Coord(x, y);
    this->direction = dir;
	this->intensity = 1.0;
	this->temp_layer = false;
	this->previous_layer = nullptr;
}

Point::Point(Coordinate& pos, double dir) {
	this->position = pos;
	this->direction = dir;
	this->temp_layer = false;
	this->intensity = 1.0;
	this->previous_layer = nullptr;
}

int Point::Move(double magnitude) {
	if (this->direction != this->prev_direction) {
		this->prev_direction = this->direction;
		this->movement_cos = cos(this->direction * M_PI);
		this->movement_sin = sin(this->direction * M_PI);
	}
    this->position += Coordinate(magnitude * this->movement_cos, magnitude * this->movement_sin);
	return 0;
}

bool Point::LayerChanged(void* layer) {
	void* p = this->previous_layer;
	this->previous_layer = layer;
	bool result = (layer != p && p != nullptr);
	if (result && this->temp_layer) {
		this->temp_layer = false;
		return false;
	}
	return result;
}

void* Point::GetPreviousLayer() {
	return this->previous_layer;
}

void Point::SetTempLayer() {
	this->temp_layer = true;
}

double Point::GetIntensity() {
    return this->intensity;
}

void Point::ManipulateIntensity(double coe) {
	this->intensity *= coe;
}
