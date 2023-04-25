#include <Layer.h>
#include <Log.h>
#include <ColorTable.h>

Layer air_layer = Layer();

Layer::Layer() {
	this->bottom_depth = 0;
	this->bulk_modulus = 0;
	this->density = 0;
	this->shear_modulus = 0;
	this->top_depth = 0;
}

Layer::Layer(const Layer& layer) {
	this->bottom_depth = layer.bottom_depth;
	this->bulk_modulus = layer.bulk_modulus;
	this->density = layer.density;
	this->shear_modulus = layer.shear_modulus;
	this->top_depth = layer.top_depth;
}

double Layer::PWaveVelocity() {
	if (this->density == 0) return 0;
    return sqrt((this->bulk_modulus + 4.0 / 3.0 * this->shear_modulus) / this->density);
}

double Layer::SWaveVelocity() {
	if (this->density == 0) return 0;
	return sqrt(this->shear_modulus / this->density);
}

LayerSet::LayerSet(Json::Value& config) {
	this->layers.clear();
    double prev_depth = 0.0;
    if (config.isMember("Layer")) {
        for (int i = 0; i < config["Layer"].size(); i++) {
            Layer lay;
            lay.bulk_modulus = config["Layer"][i]["bulk_modulus"].asDouble();
            lay.shear_modulus = config["Layer"][i]["shear_modulus"].asDouble();
            lay.density = config["Layer"][i]["density"].asDouble();
            lay.top_depth = prev_depth;
            string thickness = config["Layer"][i]["thickness"].asString();
            if (thickness == "INF" || thickness == "INFINITY" || thickness == "inf" || thickness == "infinity") {
                lay.bottom_depth = numeric_limits<double>::infinity();
            }
            else {
                lay.bottom_depth = prev_depth + config["Layer"][i]["thickness"].asDouble();
            }
            prev_depth = lay.bottom_depth;
			Log::PrintDebugLog("LayerSet", "Constructor", to_string(prev_depth));
			this->layers.push_back(lay);
        }
    }
    else {
        Log::PrintSystemLog("Layers does not exist.");
    }
}

Layer* LayerSet::operator[](Coordinate pos) {
    for (int i = 0; i < this->layers.size(); i++) {
        if (pos.y < this->layers[i].bottom_depth && pos.y >= this->layers[i].top_depth) {
            return &layers[i];
        }
    }
    return &air_layer;
}

int LayerSet::Rendering(Window* win, double zoom) {
	auto color = ColorTable::value.at(ColorTable::LAYER);
	SDL_SetRenderDrawColor(win->GetRenderer(), color.r, color.g, color.b, 255);
	if (!this->layers.empty()) {
		for (int i = 0; i < this->layers.size(); i++) {
			SDL_RenderDrawLineF(win->GetRenderer(), 0, zoom * this->layers[i].top_depth, win->GetWindowData().width, zoom * this->layers[i].top_depth);
		}
		SDL_RenderDrawLineF(win->GetRenderer(), 0, zoom * this->layers.back().bottom_depth, win->GetWindowData().width, zoom * this->layers.back().bottom_depth);
	}
	return 0;
}
