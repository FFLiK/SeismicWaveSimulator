#pragma once
#include <SDL.h>
#include <vector>
#include <json/json.h>
#include <Coordinate.h>
using namespace std;


class Layer {
public:
	double bulk_modulus, shear_modulus, density;
	double top_depth, bottom_depth;

	double PWaveVelocity();
	double SWaveVelocity();
};

class LayerSet {
private:
	vector<Layer> layers;

public:
	LayerSet(Json::Value &config);
	Layer operator[](Coordinate pos);

	int Rendering(SDL_Renderer* ren, double zoom);
};