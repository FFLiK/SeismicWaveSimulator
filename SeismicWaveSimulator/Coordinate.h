#pragma once
#include <string>

class RefractionData {
public:
	RefractionData();
	bool critical_refraction_root_wave;
	double out_direction;
};

class Coordinate {
public:
	double x;
	double y;
	
	Coordinate();
	Coordinate(double x, double y);
	Coordinate(const Coordinate& x);

	Coordinate operator+(const Coordinate operand);
	Coordinate operator-(const Coordinate operand);
	Coordinate operator*(const double operand);

	Coordinate operator+=(const Coordinate operand);
	Coordinate operator-=(const Coordinate operand);
	Coordinate operator*=(const double operand);

	std::string to_str();
};

using Coord = Coordinate;

class Point {
public:
	Point();
	Point(const Point& point);
	Point(double x, double y, double dir);
	Point(Coordinate& pos, double dir);
	
	int Move(double magnitude);

	double direction;
	Coordinate position;

	bool LayerChanged(void* layer);
	void* GetPreviousLayer();
	void SetTempLayer();

	double GetIntensity();
	void ManipulateIntensity(double coe);

	RefractionData refraction_data;

private:
	void* previous_layer;
	double intensity;
	bool temp_layer;

	double movement_cos;
	double movement_sin;

	double prev_direction;

	double distance;
};