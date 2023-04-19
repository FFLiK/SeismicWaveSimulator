#pragma once

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
};

using Coord = Coordinate;

class Point {
public:
	Point();
	Point(double x, double y, double dir);
	Point(Coordinate& pos, double dir);
	
	int Move(double magnitude);

	double direction;
	Coordinate position;
};