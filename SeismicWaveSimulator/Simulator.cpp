#include <Simulator.h>
#include <FileManager.h>
#include <ColorTable.h>
#include <Window.h>
#include <Log.h>
#include <atomic>
#include "Config.h"

double Expo(double x) {
	return x == 1 ? 1 : 1 - pow(2, -10 * x);
}

double SnellLaw(double theta, double v_in, double v_out) {
	double abs_theta = theta;
	if (theta >= 1) theta -= 1;
	theta = abs(0.5 - theta);

	double v = v_out / v_in * sin(theta * M_PI);
	if (abs(v - 1.0) < EPSILON) v = 1.0;
	else if (abs(v + 1.0) < EPSILON) v = -1.0;
	double ret = asin(v) / M_PI;
	ret = 0.5 - ret;
	if (abs_theta <= 0.5);
	else if (abs_theta > 0.5 && abs_theta <= 1.0) ret = 1.0 - ret;
	else if (abs_theta > 1.0 && abs_theta <= 1.5) ret += 1.0;
	else if (abs_theta > 1.5) ret = 2.0 - ret;
	while (ret >= 2.0) ret -= 2.0;
	while (ret < 0.0) ret += 2.0;
	return ret;
}

Simulator::Simulator(string config_path) {
	Log::PrintDebugLog("Simulator", "Contructor", "생성자 호출");
    Json::Value config = FileManager::GetJsonFile(config_path);
    layer_set = new LayerSet(config);
	this->receiver = make_shared<vector<Receiver>>();
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
		for (int i = 0; i < index; i++) {
			angle = 1 + SnellLaw(0, this->layer_set->layers[i].PWaveVelocity(), this->layer_set->layers[index].PWaveVelocity());
			new_wave->push_back(Point(hypocenter, angle));

			angle = 1 + SnellLaw(1, this->layer_set->layers[i].PWaveVelocity(), this->layer_set->layers[index].PWaveVelocity());
			new_wave->push_back(Point(hypocenter, angle));
		}

		atomic_store(&(this->p_refracted_wave), new_wave);
		new_wave.reset();
	}
	#endif
	{
		shared_ptr<vector<Point>> new_wave = make_shared<vector<Point>>();
		#if OCCUR_S_WAVE 
		for (int i = 0; i < SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER; i++) {
			new_wave->push_back(Point(hypocenter, (double)i / (double)SEISMIC_WAVE_INITIAL_PARTICLE_NUMBER * 2));
		}
		#endif
		atomic_store(&(this->s_wave), new_wave);
		new_wave.reset();
	}
	#if OCCUR_S_REFRACTED_WAVE 
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
		for (int i = 0; i < index; i++) {
			angle = 1 + SnellLaw(0, this->layer_set->layers[i].PWaveVelocity(), this->layer_set->layers[index].PWaveVelocity());
			new_wave->push_back(Point(hypocenter, angle));

			angle = 1 + SnellLaw(1, this->layer_set->layers[i].PWaveVelocity(), this->layer_set->layers[index].PWaveVelocity());
			new_wave->push_back(Point(hypocenter, angle));
		}

		atomic_store(&(this->s_refracted_wave), new_wave);
		new_wave.reset();
	}
	#endif

	shared_ptr<vector<Receiver>> receiver = atomic_load(&this->receiver);
	for (int i = 0; i < receiver->size(); i++) {
		(*receiver)[i].received = false;
	}
	atomic_store(&(this->receiver), receiver);
    return 0;
}

int Simulator::InstallReceiver(Coordinate position) {
	#if USE_RECEIVER
	shared_ptr<vector<Receiver>> receiver = atomic_load(&this->receiver);
	bool chk = true;
	for (int i = 0; i < receiver->size(); i++) {
		if (position.x + RECEIVER_SIZE >= (*receiver)[i].pos.x - RECEIVER_SIZE && position.x - RECEIVER_SIZE <= (*receiver)[i].pos.x + RECEIVER_SIZE) {
			if (position.y + RECEIVER_SIZE >= (*receiver)[i].pos.y - RECEIVER_SIZE && position.y - RECEIVER_SIZE <= (*receiver)[i].pos.y + RECEIVER_SIZE) {
				chk = false;
				if (position.x >= (*receiver)[i].pos.x - RECEIVER_SIZE && position.x <= (*receiver)[i].pos.x + RECEIVER_SIZE) {
					if (position.y >= (*receiver)[i].pos.y - RECEIVER_SIZE && position.y <= (*receiver)[i].pos.y + RECEIVER_SIZE) {
						(*receiver).erase((*receiver).begin() + i);
						break;
					}
				}
			}
		}
	}
	if (chk) {
		Receiver r;
		r.pos = position;
		r.received = false;
		receiver->push_back(r);
	}
	atomic_store(&(this->receiver), receiver);
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
	this->Calculate(CAL_S_WAVE | CAL_REFRACTED, delta_time);
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
	this->RenderWave(CAL_S_WAVE | CAL_REFRACTED, win, zoom);
	#endif
	this->layer_set->Rendering(win, zoom);

	shared_ptr<vector<Receiver>> receiver = atomic_load(&this->receiver);
	for (int i = 0; i < receiver->size(); i++) {
		Color::RGB color = ColorTable::value.at(ColorTable::RECEIVER);
		Color::HSV hsv = color;
		hsv.h -= 15 * i;
		while (hsv.h < 0) hsv.h += 360;
		color = hsv;
		(*receiver)[i].Rendering(win, zoom, color);
		SDL_SetRenderDrawColor(win->GetRenderer(), color.r, color.g, color.b, 255);
		SDL_FRect r;
		r.x = ((*receiver)[i].pos.x - RECEIVER_SIZE) * zoom;
		r.y = ((*receiver)[i].pos.y - RECEIVER_SIZE) * zoom;
		r.w = RECEIVER_SIZE * 2 * zoom;
		r.h = RECEIVER_SIZE * 2 * zoom;
		SDL_RenderFillRectF(win->GetRenderer(), &r);
	}

	SDL_SetRenderDrawColor(win->GetRenderer(), 0, 0, 0, 255);
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
		shared_ptr<vector<Receiver>> receiver = atomic_load(&this->receiver);
		shared_ptr<vector<Point>> new_wave_vec = make_shared<vector<Point>>();
		shared_ptr<vector<Point>> new_wave_vec_for_cr = make_shared<vector<Point>>();
		for (register int i = 0; i < wave->size(); i++) {
			Coord wave_pos = (*wave)[i].position;
			for (register int j = 0; j < receiver->size(); j++) {
				if (!(*receiver)[j].received) {
					Coord re_pos = (*receiver)[j].pos;
					if (wave_pos.x >= re_pos.x - RECEIVER_SIZE && wave_pos.x <= re_pos.x + RECEIVER_SIZE) {
						if (wave_pos.y >= re_pos.y - RECEIVER_SIZE && wave_pos.y <= re_pos.y + RECEIVER_SIZE) {
							(*receiver)[j].record = ((*wave)[i]);
							(*receiver)[j].received = true;
							(*receiver)[j].record.AddHistory();
						}
					}
				}
			}

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
						if (abs(new_wave.direction) < EPSILON) {
							new_wave.direction = 0;
							new_wave.refraction_data.critical_refraction_root_wave = true;
							new_wave.refraction_data.out_direction = 2.0 - prev_direction;
						}
						else if (abs(new_wave.direction - 1) < EPSILON) {
							new_wave.direction = 1;
							new_wave.refraction_data.critical_refraction_root_wave = true;
							new_wave.refraction_data.out_direction = 2.0 - prev_direction;
						}
					}
					new_wave.ManipulateIntensity(1 - reflection_coefficient);
					new_wave_vec->push_back(new_wave);

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
					new_wave_vec->push_back(reflected);
				}
				else {
					if ((wavetype & CAL_REFRACTED) && ((*wave)[i].refraction_data.critical_refraction_root_wave)) {
						Point refracted = (*wave)[i];
						refracted.direction = refracted.refraction_data.out_direction;
						refracted.refraction_data = RefractionData();
						refracted.SetTempLayer();
						refracted.AddHistory();
						new_wave_vec->push_back(refracted);
					}
					new_wave_vec->push_back((*wave)[i]);
				}
			}
		}
		wave.reset();
		for (register int i = 0; i < new_wave_vec->size(); i++) {
			double velocity = 1;
			if (wavetype & CAL_P_WAVE) velocity = (*this->layer_set)[(*new_wave_vec)[i].position]->PWaveVelocity();
			else if (wavetype & CAL_S_WAVE) velocity = (*this->layer_set)[(*new_wave_vec)[i].position]->SWaveVelocity();
			double magnitude = velocity * delta_time;
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
		atomic_store(&this->receiver, receiver);

		new_wave_vec.reset();
		new_wave_vec_for_cr.reset();
		receiver.reset();
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

int Receiver::Rendering(Window* win, double zoom, Color::RGB color) {
	if (this->received) {
		this->record.RenderHistory(win, zoom, color);
	}
	/*for (int i = 0; i < this->record.size(); i++) {
		if (i >= 10) break;
		Color::HSV hsv = color;
		hsv.v -= i * 10;
		color = hsv;
		this->record[i].RenderHistory(win, zoom, color);
	}*/
	return 0;
}
