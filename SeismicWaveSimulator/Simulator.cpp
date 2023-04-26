#include <Simulator.h>
#include <FileManager.h>
#include <ColorTable.h>
#include <Window.h>
#include <Log.h>
#include <atomic>

#define SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER 25000
#define INTENSITY_THRESHOLD 0.1

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
	shared_ptr<vector<Point>> new_p_wave = make_shared<vector<Point>>();
    for (int i = 0; i < SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER; i++) {
		new_p_wave->push_back(Point(hypocenter, (double)i / (double)SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER * 2));
	}
	atomic_store(&(this->p_wave), new_p_wave);
	new_p_wave.reset();
    return 0;
}

double SnellLaw(double theta, double v_in, double v_out) {
	double abs_theta = theta;
	if (theta >= 1) theta -= 1;
	theta = abs(0.5 - theta);
	double ret = asin(v_out / v_in * sin(theta * M_PI)) / M_PI;
	ret = 0.5 - ret;
	if (abs_theta <= 0.5);
	else if (abs_theta > 0.5 && abs_theta <= 1.0) ret = 1.0 - ret;
	else if (abs_theta > 1.0 && abs_theta <= 1.5) ret += 1.0;
	else if (abs_theta > 1.5) ret = 2.0 - ret;
	return ret;
}

int Simulator::Update(double delta_time) {
	shared_ptr<vector<Point>> wave = this->p_wave;
	if (wave) {
		shared_ptr<vector<Point>> new_p_wave = make_shared<vector<Point>>();
		for (register int i = 0; i < wave->size(); i++) {
			Layer* layer = (*this->layer_set)[(*wave)[i].position];
			if ((*wave)[i].position.x >= 0 && 
				(*wave)[i].position.y >= 0 && 
				(*wave)[i].GetIntensity() > INTENSITY_THRESHOLD) {
				Layer* previous_layer = (Layer*)(*wave)[i].GetPreviousLayer();
				if ((*wave)[i].LayerChanged(layer)) {
					//Refraction
					double reflection_coefficient = abs((layer->PWaveVelocity() - previous_layer->PWaveVelocity()) / (layer->PWaveVelocity() + previous_layer->PWaveVelocity()));
					Point new_wave = (*wave)[i];
					new_wave.direction = SnellLaw(new_wave.direction, previous_layer->PWaveVelocity(), layer->PWaveVelocity());
					new_wave.ManipulateIntensity(1 - reflection_coefficient);
					new_p_wave->push_back(new_wave);

					//Create Reflected Wave
					Point reflected = (*wave)[i];
					reflected.direction = -reflected.direction + 2.0;
					reflected.SetTempLayer();
					reflected.ManipulateIntensity(reflection_coefficient);
					Layer* is_air = (*this->layer_set)[reflected.position];
					if (is_air->density == 0) {
						if (is_air->top_depth) {
							reflected.position.y = is_air->top_depth - 1;
						}
						else {
							reflected.position.y = is_air->bottom_depth + 1;
						}
					}
					new_p_wave->push_back(reflected);
				}
				else {
					new_p_wave->push_back((*wave)[i]);
				}
			}
		}
		for (register int i = 0; i < new_p_wave->size(); i++) {
			double magnitude = (*this->layer_set)[(*new_p_wave)[i].position]->PWaveVelocity() * delta_time;
			(*new_p_wave)[i].Move(magnitude);
		}
		atomic_store(&(this->p_wave), new_p_wave);
		new_p_wave.reset();
	}
    return 0;
}

int Simulator::Rendering(Window* win, double zoom) {
	shared_ptr<vector<Point>> wave;
	Color::RGB color(0,0,0);
	color = ColorTable::value.at(ColorTable::P_WAVE);
	wave = this->p_wave;
	if (wave) {
		for (int i = 0; i < wave->size(); i++) {
			Color::HSV hsv(color);
			hsv.v = 80.0 * (*wave)[i].GetIntensity() + 20.0;
			Color::RGB c(hsv);
			SDL_SetRenderDrawColor(win->GetRenderer(), c.r, c.g, c.b, 255);
			SDL_FRect rect;
			rect.x = (*wave)[i].position.x * zoom - 0.5;
			rect.y = (*wave)[i].position.y * zoom - 0.5;
			rect.w = 2.0;
			rect.h = 2.0;
			SDL_RenderDrawRectF(win->GetRenderer(), &rect);
		}
		//Log::PrintSystemLog("P Wave Particle Size = " + to_string(wave->size()));
		wave.reset();
	}

	this->layer_set->Rendering(win, zoom);
	return 0;
}
