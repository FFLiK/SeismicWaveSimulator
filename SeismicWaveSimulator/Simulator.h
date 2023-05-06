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
public:
	Coord pos;
	Point record;
	bool received = false;

	int Rendering(Window* win, double zoom, Color::RGB color);
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
};