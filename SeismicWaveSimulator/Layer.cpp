#include <Layer.h>
#include <Log.h>
#include <ColorTable.h>

#define AIR_LAYER Layer()

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

	if (this->density == 0) this->p_wave_speed = 0;
	else this->p_wave_speed = sqrt((this->bulk_modulus + 4.0 / 3.0 * this->shear_modulus) / this->density);
	
	if (this->density == 0) this->s_wave_speed = 0;
	else this->s_wave_speed = sqrt(this->shear_modulus / this->density);
}

double Layer::PWaveVelocity() {
	return this->p_wave_speed;
}

double Layer::SWaveVelocity() {
	return this->s_wave_speed;
}

LayerSet::LayerSet(Json::Value& config) {
	#if INF_LAYER
	int num = 500;
	int delta = 1080 / num + 1;
	for (int i = 0; i < num; i++) {
		Layer lay;
		lay.bulk_modulus = (i + 1) * (i + 1);
		lay.shear_modulus = 0;
		lay.density = 1000;
		lay.top_depth = i * delta;
		lay.bottom_depth = (i + 1) * delta;
		this->layers.push_back(lay);
	}
	Layer lay;
	lay.bulk_modulus = (num + 1) * (num + 1);
	lay.shear_modulus = 0;
	lay.density = 1000;
	lay.top_depth = num * delta;
	lay.bottom_depth = numeric_limits<double>::infinity();
	this->layers.push_back(lay);
	#else
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
			this->layers.push_back(lay);
        }
    }
    else {
        Log::PrintSystemLog("Layers does not exist.");
    }
	#endif
}

Layer* LayerSet::operator[](Coordinate pos) {
	int index = this->GetLayerIndex(pos);
	if (index == -1) {
		Layer air = AIR_LAYER;
		return &air;
	}
	return &this->layers[index];
}

int LayerSet::GetLayerIndex(Coordinate pos) {
	for (int i = 0; i < this->layers.size(); i++) {
		if (pos.y < this->layers[i].bottom_depth && pos.y >= this->layers[i].top_depth) {
			return i;
		}
	}
	return -1;
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
