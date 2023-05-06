#pragma once
#include <SDL.h>
#include <iostream>
#include <vector>
#include <Coordinate.h>
#include <Layer.h>
#include <Window.h>
#include <memory>
using namespace std;

class Receiver {
	vector<Coord> path;
public:
	Coord pos;
	void FindPath(Coord begin, Coord end, double direction, LayerSet layer);
	int Rendering(Window* win, double zoom, Color::RGB color);
	bool Received();
	void Clear();
};

class Simulator {
public:
	Simulator(string config_path);
	~Simulator();

	int OccurEarthquake(Coordinate hypocenter);
	int InstallReceiver(Coordinate position);

	int Update(double delta_time);
	int Rendering(Window* win, double zoom);

private:
	LayerSet* layer_set;
	shared_ptr<vector<Point>> p_wave;
	shared_ptr<vector<Point>> s_wave;
	shared_ptr<vector<Point>> p_refracted_wave;
	shared_ptr<vector<Point>> s_refracted_wave;

	void Calculate(int wavetype, double delta_time);
	void RenderWave(int wavetype, Window* win, double zoom);

	shared_ptr<vector<Receiver>> receiver;

	Coord hypocenter;
};