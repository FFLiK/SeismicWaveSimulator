#pragma once
#include <SDL.h>
#include <vector>
#include <json/json.h>
#include <Coordinate.h>
#include <Window.h>
using namespace std;


class Layer {
public:
	Layer();
	Layer(const Layer& layer);

	double bulk_modulus, shear_modulus, density;
	double top_depth, bottom_depth;

	double PWaveVelocity();
	double SWaveVelocity();

private:
	double p_wave_speed;
	double s_wave_speed;
};

class LayerSet {
private:
	vector<Layer> layers;

public:
	LayerSet(Json::Value &config);
	Layer* operator[](Coordinate pos);

	int Rendering(Window* win, double zoom);
};