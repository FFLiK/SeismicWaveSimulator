#include <Layer.h>
#include <Log.h>
#include <ColorTable.h>

const Layer air_layer = { 0,0,0,0,0 };

double Layer::PWaveVelocity() {
    return sqrt((this->bulk_modulus + 4.0 / 3.0 * this->shear_modulus) / this->density);
}

double Layer::SWaveVelocity() {
    return sqrt(this->shear_modulus / this->density);
}

LayerSet::LayerSet(Json::Value& config) {
    double prev_depth = 0.0;
    if (config.isMember("Layer")) {
        for (int i = 0; i < config["Layer"].size(); i++) {
            Layer lay;
            lay.bulk_modulus = config["bulk_modulus"].asDouble();
            lay.shear_modulus = config["shear_modulus"].asDouble();
            lay.density = config["density"].asDouble();
            lay.top_depth = prev_depth;
            string thickness = config["thickness"].asString();
            if (thickness == "INF" || thickness == "INFINITY" || thickness == "inf" || thickness == "infinity") {
                lay.bottom_depth = numeric_limits<double>::infinity();
            }
            else {
                lay.bottom_depth = prev_depth + config["thickness"].asDouble();
            }
            prev_depth = lay.bottom_depth;
        }
    }
    else {
        Log::PrintSystemLog("Layers does not exist.");
    }
}

Layer LayerSet::operator[](Coordinate pos) {
    for (int i = 0; i < this->layers.size(); i++) {
        if (pos.y < this->layers[i].bottom_depth && pos.y >= this->layers[i].top_depth) {
            return layers[i];
        }
    }
    return air_layer;
}

int LayerSet::Rendering(SDL_Renderer* ren, double zoom) {
	auto color = ColorTable::value.at(ColorTable::LAYER);
	SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, 255);
	for (int i = 0; i < this->layers.size(); i++) {
		SDL_RenderDrawLineF(ren, 0, zoom * this->layers[i].top_depth, 1280, zoom * this->layers[i].top_depth);
	}
	SDL_RenderDrawLineF(ren, 0, zoom * this->layers.back().bottom_depth, 1280, zoom * this->layers.back().bottom_depth);
    return 0;
}
