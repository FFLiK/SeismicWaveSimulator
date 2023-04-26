#pragma once
#include <SDL.h>
#include <iostream>
#include <vector>
#include <Coordinate.h>
#include <Layer.h>
#include <Window.h>
#include <memory>
using namespace std;


class Simulator {
public:
	Simulator(string config_path);
	~Simulator();

	int OccurEarthquake(Coordinate hypocenter);

	int Update(double delta_time);
	int Rendering(Window* win, double zoom);

private:
	LayerSet* layer_set;
	shared_ptr<vector<Point>> p_wave;
	shared_ptr<vector<Point>> s_wave;
};