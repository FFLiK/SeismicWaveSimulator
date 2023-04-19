#include <Simulator.h>
#include <FileManager.h>
#include <ColorTable.h>

#define SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER 10000

Simulator::Simulator(string config_path) {
    Json::Value config = FileManager::GetJsonFile(config_path);
    layer_set = new LayerSet(config);
}

Simulator::~Simulator() {
    delete layer_set;
}

int Simulator::OccurEarthquake(Coordinate hypocenter) {
    this->p_wave.clear();
    for (int i = 0; i < SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER; i++) {
        this->p_wave.push_back(Point(hypocenter, (double)i / (double)SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER));
    }
    return 0;
}

int Simulator::Update() {
    for (int i = 0; i < this->p_wave.size(); i++) {
        double magnitude = (*this->layer_set)[this->p_wave[i].position].PWaveVelocity();
        this->p_wave[i].Move(magnitude);
    }
    return 0;
}

int Simulator::Rendering(SDL_Renderer* ren, double zoom) {
	this->layer_set->Rendering(ren, zoom);
	
	auto color = ColorTable::value.at(ColorTable::P_WAVE);
	SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, 255);
	for (int i = 0; i < this->p_wave.size(); i++) {
		SDL_RenderDrawPointF(ren, this->p_wave[i].position.x, this->p_wave[i].position.y);
	}
	
	color = ColorTable::value.at(ColorTable::S_WAVE);
	SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, 255);
	for (int i = 0; i < this->s_wave.size(); i++) {
		SDL_RenderDrawPointF(ren, this->s_wave[i].position.x, this->s_wave[i].position.y);
	}
	return 0;
}
