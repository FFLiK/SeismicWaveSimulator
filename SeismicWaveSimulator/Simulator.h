#pragma once
#include <SDL.h>
#include <iostream>
#include <vector>
#include <Coordinate.h>
#include <Layer.h>
using namespace std;


class Simulator {
public:
	Simulator(string config_path);
	~Simulator();

	int OccurEarthquake(Coordinate hypocenter);

	int Update();
	int Rendering(SDL_Renderer* ren, double zoom);

private:
	LayerSet* layer_set;

	vector<Point> p_wave;
	vector<Point> s_wave;
};