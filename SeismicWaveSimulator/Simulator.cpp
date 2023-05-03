#include <Simulator.h>
#include <FileManager.h>
#include <ColorTable.h>
#include <Window.h>
#include <Log.h>
#include <atomic>

#define SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER 25000
#define INTENSITY_THRESHOLD 0.1

#define OCCUR_P_WAVE true
#define OCCUR_P_REFRACTED_WAVE true
#define OCCUR_S_WAVE false
#define OCCUR_S_REFRACTED_WAVE false

#define CAL_P_WAVE 1
#define CAL_S_WAVE 2
#define CAL_REFRACTED 4

double Expo(double x) {
	return x;
	return x == 1 ? 1 : 1 - pow(10, -10 * x);
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

Simulator::Simulator(string config_path) {
	Log::PrintDebugLog("Simulator", "Contructor", "생성자 호출");
    Json::Value config = FileManager::GetJsonFile(config_path);
    layer_set = new LayerSet(config);
}

Simulator::~Simulator() {
	this->p_wave.reset();
	this->s_wave.reset();
    delete layer_set;
	Log::PrintDebugLog("Simulator", "Destructor", "소멸자 호출");
}

int Simulator::OccurEarthquake(Coordinate hypocenter) {
	{
		shared_ptr<vector<Point>> new_wave = make_shared<vector<Point>>();
		#if OCCUR_P_WAVE 
		for (int i = 0; i < SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER; i++) {
			new_wave->push_back(Point(hypocenter, (double)i / (double)SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER * 2));
		}
		#endif
		atomic_store(&(this->p_wave), new_wave);
		new_wave.reset();
	} 
	#if OCCUR_P_REFRACTED_WAVE 
	{
		shared_ptr<vector<Point>> new_wave = make_shared<vector<Point>>();
		int index = this->layer_set->GetLayerIndex(hypocenter);
		double angle = 0;
		for (int i = index + 1; i < this->layer_set->layers.size(); i++) {
			angle = SnellLaw(0, this->layer_set->layers[i].PWaveVelocity(), this->layer_set->layers[index].PWaveVelocity());
			new_wave->push_back(Point(hypocenter, angle));
			
			angle = SnellLaw(1, this->layer_set->layers[i].PWaveVelocity(), this->layer_set->layers[index].PWaveVelocity());
			new_wave->push_back(Point(hypocenter, angle));
		}

		atomic_store(&(this->p_refracted_wave), new_wave);
		new_wave.reset();
	}
	#endif
    return 0;
}

int Simulator::Update(double delta_time) {
	this->Calculate(CAL_P_WAVE, delta_time);
	this->Calculate(CAL_S_WAVE, delta_time);

	#if OCCUR_P_REFRACTED_WAVE
	this->Calculate(CAL_P_WAVE | CAL_REFRACTED, delta_time);
	#endif
	#if OCCUR_S_REFRACTED_WAVE
	this->Calculate(CAL_P_WAVE | CAL_REFRACTED, delta_time);
	#endif
    return 0;
}

int Simulator::Rendering(Window* win, double zoom) {
	this->RenderWave(CAL_P_WAVE, win, zoom);
	this->RenderWave(CAL_S_WAVE, win, zoom);

	#if OCCUR_P_REFRACTED_WAVE
	this->RenderWave(CAL_P_WAVE | CAL_REFRACTED, win, zoom);
	#endif
	#if OCCUR_S_REFRACTED_WAVE
	this->RenderWave(CAL_P_WAVE | CAL_REFRACTED, win, zoom);
	#endif
	this->layer_set->Rendering(win, zoom);
	return 0;
}

void Simulator::Calculate(int wavetype, double delta_time) {
	shared_ptr<vector<Point>> wave;
	if (wavetype & CAL_REFRACTED) {
		if(wavetype & CAL_P_WAVE) wave = atomic_load(&this->p_refracted_wave);
		else if(wavetype & CAL_S_WAVE) wave = atomic_load(&this->s_refracted_wave);
	}
	else {
		if(wavetype & CAL_P_WAVE) wave = atomic_load(&this->p_wave);
		else if(wavetype & CAL_S_WAVE) wave = atomic_load(&this->s_wave);
	}
	if (wave) {
		shared_ptr<vector<Point>> new_wave_vec = make_shared<vector<Point>>();
		shared_ptr<vector<Point>> new_wave_vec_for_cr = make_shared<vector<Point>>();
		for (register int i = 0; i < wave->size(); i++) {
			Layer* layer = (*this->layer_set)[(*wave)[i].position];
			if ((*wave)[i].position.x >= 0 &&
				(*wave)[i].position.y >= 0 &&
				Expo((*wave)[i].GetIntensity()) > INTENSITY_THRESHOLD) {
				Layer* previous_layer = (Layer*)(*wave)[i].GetPreviousLayer();
				if ((*wave)[i].LayerChanged(layer)) {
					//Refraction
					double velocity = 1;
					double prev_velocity = 1;
					if (wavetype & CAL_P_WAVE) velocity = layer->PWaveVelocity(), prev_velocity = previous_layer->PWaveVelocity();
					else if (wavetype & CAL_S_WAVE) velocity = layer->SWaveVelocity(), prev_velocity = previous_layer->SWaveVelocity();

					double reflection_coefficient = abs((velocity - prev_velocity) / (velocity + prev_velocity));
					Point new_wave = (*wave)[i];
					double prev_direction = new_wave.direction;
					new_wave.direction = SnellLaw(new_wave.direction, prev_velocity, velocity);
					if (wavetype & CAL_REFRACTED) {
						if (abs(new_wave.direction) < 0.00001) {
							new_wave.direction = 0;
							new_wave.refraction_data.critical_refraction_root_wave = true;
							new_wave.refraction_data.out_direction = 2.0 - prev_direction;
						}
						else if (abs(new_wave.direction - 1) < 0.00001) {
							new_wave.direction = 1;
							new_wave.refraction_data.critical_refraction_root_wave = true;
							new_wave.refraction_data.out_direction = 2.0 - prev_direction;
						}
					}
					new_wave.ManipulateIntensity(1 - reflection_coefficient);
					new_wave_vec->push_back(new_wave);

					//Create Reflected Wave
					if (!(wavetype & CAL_REFRACTED)) {
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
						new_wave_vec->push_back(reflected);
					}
				}
				else {
					if ((wavetype & CAL_REFRACTED) && ((*wave)[i].refraction_data.critical_refraction_root_wave)) {
						Point refracted = (*wave)[i];
						refracted.direction = refracted.refraction_data.out_direction;
						refracted.refraction_data = RefractionData();
						refracted.SetTempLayer();
						new_wave_vec_for_cr->push_back(refracted);
					}
					new_wave_vec->push_back((*wave)[i]);
				}
			}
		}
		wave.reset();
		for (register int i = 0; i < new_wave_vec->size(); i++) {
			double magnitude = (*this->layer_set)[(*new_wave_vec)[i].position]->PWaveVelocity() * delta_time;
			(*new_wave_vec)[i].Move(magnitude);
		}
		if (wavetype & CAL_REFRACTED) {
			bool cr = false;
			if (!new_wave_vec_for_cr->empty()) {
				shared_ptr<vector<Point>> wave;
				if (wavetype & CAL_P_WAVE) wave = atomic_load(&this->p_wave);
				else if (wavetype & CAL_S_WAVE) wave = atomic_load(&this->s_wave);
				if (wave) {
					for (int i = 0; i < wave->size(); i++) {
						new_wave_vec_for_cr->push_back((*wave)[i]);
					}
				}
				cr = true;
				wave.reset();
			}

			if (wavetype & CAL_P_WAVE) {
				atomic_store(&this->p_refracted_wave, new_wave_vec);
				if (cr) {
					atomic_store(&this->p_wave, new_wave_vec_for_cr);
				}
			}
			else if (wavetype & CAL_S_WAVE) {
				atomic_store(&this->s_refracted_wave, new_wave_vec);
				if (cr) {
					atomic_store(&this->s_wave, new_wave_vec_for_cr);
				}
			}
		}
		else {
			if (wavetype & CAL_P_WAVE) atomic_store(&this->p_wave, new_wave_vec);
			else if (wavetype & CAL_S_WAVE) atomic_store(&this->s_wave, new_wave_vec);
		}
		new_wave_vec.reset();
		new_wave_vec_for_cr.reset();
	}
}

void Simulator::RenderWave(int wavetype, Window* win, double zoom) {
	shared_ptr<vector<Point>> wave;
	Color::RGB color(0, 0, 0);
	if (wavetype & CAL_REFRACTED) {
		if (wavetype & CAL_P_WAVE) {
			wave = atomic_load(&this->p_refracted_wave);
			color = ColorTable::value.at(ColorTable::P_REFRACTED_WAVE);
		}
		else if (wavetype & CAL_S_WAVE) {
			wave = atomic_load(&this->s_refracted_wave);
			color = ColorTable::value.at(ColorTable::S_REFRACTED_WAVE);
		}
	}
	else {
		if (wavetype & CAL_P_WAVE) {
			wave = atomic_load(&this->p_wave);
			color = ColorTable::value.at(ColorTable::P_WAVE);
		}
		else if (wavetype & CAL_S_WAVE) {
			wave = atomic_load(&this->s_wave);
			color = ColorTable::value.at(ColorTable::S_WAVE);
		}
	}
	if (wave) {
		for (int i = 0; i < wave->size(); i++) {
			Color::HSV hsv(color);
			hsv.v = 100.0 * Expo((*wave)[i].GetIntensity());
			Color::RGB c(hsv);
			SDL_SetRenderDrawColor(win->GetRenderer(), c.r, c.g, c.b, 255);
			SDL_FRect rect;
			rect.x = (*wave)[i].position.x * zoom - 0.5;
			rect.y = (*wave)[i].position.y * zoom - 0.5;
			rect.w = 2.0;
			rect.h = 2.0;
			SDL_RenderFillRectF(win->GetRenderer(), &rect);
			/*SDL_FPoint p;
			p.x = (*wave)[i].position.x * zoom;
			p.y = (*wave)[i].position.y * zoom;
			SDL_RenderDrawPointF(win->GetRenderer(), p.x, p.y);*/
		}
		wave.reset();
	}
}
