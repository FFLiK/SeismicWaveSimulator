#include <Simulator.h>
#include <FileManager.h>
#include <ColorTable.h>
#include <Window.h>
#include <Log.h>

#define SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER 100000

Simulator::Simulator(string config_path) {
	Log::PrintDebugLog("Simulator", "Contructor", "생성자 호출");
    Json::Value config = FileManager::GetJsonFile(config_path);
    layer_set = new LayerSet(config);
	this->p_wave = make_shared<vector<Point>>();
	this->s_wave = make_shared<vector<Point>>();
}

Simulator::~Simulator() {
	this->p_wave.reset();
	this->s_wave.reset();
    delete layer_set;
	Log::PrintDebugLog("Simulator", "Destructor", "소멸자 호출");
}

int Simulator::OccurEarthquake(Coordinate hypocenter) {
    this->p_wave->clear();
    for (int i = 0; i < SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER; i++) {
        this->p_wave->push_back(Point(hypocenter, (double)i / (double)SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER * 2));
	}
    return 0;
}

int Simulator::Update(double velocity) {
	if (this->p_wave) {
		shared_ptr<vector<Point>> new_p_wave = make_shared<vector<Point>>();
		for (int i = 0; i < this->p_wave->size(); i++) {
			if ((*this->p_wave)[i].position.x >= 0 && (*this->p_wave)[i].position.y >= 0) {
				Layer* layer = (*this->layer_set)[(*this->p_wave)[i].position];
				Layer* previous_layer = (Layer*)(*this->p_wave)[i].GetPreviousLayer();
				if ((*this->p_wave)[i].LayerChanged(layer)) {
					//Refraction
					Point new_wave = (*this->p_wave)[i];
					double theta1 = new_wave.direction;
					if (theta1 >= 1) theta1 -= 1;
					theta1 = abs(0.5 - theta1);
					double theta2 = asin(layer->PWaveVelocity() / previous_layer->PWaveVelocity() * sin(theta1 * M_PI)) / M_PI;
					theta2 = 0.5 - theta2;
					if (new_wave.direction <= 0.5);
					else if (new_wave.direction > 0.5 && new_wave.direction <= 1.0) theta2 = 1.0 - theta2;
					else if (new_wave.direction > 1.0 && new_wave.direction <= 1.5) theta2 += 1.0;
					else if (new_wave.direction > 1.5) theta2 = 2.0 - theta2;
					new_wave.direction = theta2;
					new_p_wave->push_back(new_wave);

					//Create Reflected Wave
					Point reflected = (*this->p_wave)[i];
					reflected.direction = -reflected.direction + 2.0;
					while (!reflected.LayerChanged((*this->layer_set)[reflected.position])) {
						reflected.Move(1.0);
					}
					new_p_wave->push_back(reflected);
				}
				else {
					new_p_wave->push_back((*this->p_wave)[i]);
				}
			}
		}
		for (int i = 0; i < new_p_wave->size(); i++) {
			double magnitude = (*this->layer_set)[(*new_p_wave)[i].position]->PWaveVelocity() * velocity;
			(*new_p_wave)[i].Move(magnitude);
		}
		this->p_wave = new_p_wave;
	}
    return 0;
}

int Simulator::Rendering(Window* win, double zoom) {
	this->layer_set->Rendering(win, zoom);

	shared_ptr<vector<Point>> wave;
	Color::RGB color(0,0,0);
	color = ColorTable::value.at(ColorTable::P_WAVE);
	if (this->p_wave) {
		wave = this->p_wave;
		SDL_SetRenderDrawColor(win->GetRenderer(), color.r, color.g, color.b, 255);
		for (int i = 0; i < wave->size(); i++) {
			SDL_RenderDrawPointF(win->GetRenderer(), (*wave)[i].position.x * zoom, (*this->p_wave)[i].position.y * zoom);
		}
		wave.reset();
	}
	
	wave = this->s_wave;
	color = ColorTable::value.at(ColorTable::S_WAVE);
	SDL_SetRenderDrawColor(win->GetRenderer(), color.r, color.g, color.b, 255);
	for (int i = 0; i < wave->size(); i++) {
		SDL_RenderDrawPointF(win->GetRenderer(), (*wave)[i].position.x * zoom, (*this->p_wave)[i].position.y * zoom);
	}
	wave.reset();
	return 0;
}
