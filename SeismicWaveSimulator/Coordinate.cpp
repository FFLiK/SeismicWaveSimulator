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
	this->distance = 0;
	this->prev_direction = -1;
	this->movement_cos = 0;
	this->movement_sin = 0;
	this->refraction_data = RefractionData();
}

Point::Point(const Point& point) {
	this->position = point.position;
	this->direction = point.direction;
	this->intensity = point.intensity;
	this->temp_layer = point.temp_layer;
	this->previous_layer = point.previous_layer;
	this->distance = point.distance;
	this->prev_direction = point.prev_direction;
	this->movement_cos = point.movement_cos;
	this->movement_sin = point.movement_sin;
	this->refraction_data = point.refraction_data;
}

Point::Point(double x, double y, double dir) {
    this->position = Coord(x, y);
    this->direction = dir;
	this->intensity = 1.0;
	this->temp_layer = false;
	this->previous_layer = nullptr;
	this->distance = 0;
	this->prev_direction = -1;
	this->movement_cos = 0;
	this->movement_sin = 0;
	this->refraction_data = RefractionData();
}

Point::Point(Coordinate& pos, double dir) {
	this->position = pos;
	this->direction = dir;
	this->temp_layer = false;
	this->intensity = 1.0;
	this->previous_layer = nullptr;
	this->distance = 0;
	this->prev_direction = -1;
	this->movement_cos = 0;
	this->movement_sin = 0;
	this->refraction_data = RefractionData();
}

int Point::Move(double magnitude) {
	if (this->direction != this->prev_direction) {
		this->prev_direction = this->direction;
		this->movement_cos = cos(this->direction * M_PI);
		this->movement_sin = sin(this->direction * M_PI);
	}
    this->position += Coordinate(magnitude * this->movement_cos, magnitude * this->movement_sin);
	this->distance += magnitude / 10000.0;
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
	if (!this->distance) return this->intensity;
    return this->intensity / (1 + this->distance);
}

void Point::ManipulateIntensity(double coe) {
	this->intensity *= coe;
}

void Point::RenderHistory(Window* win, double zoom, Color::RGB color) {
	SDL_SetRenderDrawColor(win->GetRenderer(), color.r, color.g, color.b, 255);
}

RefractionData::RefractionData() {
	this->critical_refraction_root_wave = false;
	this->out_direction = 0;
}
