#include "MainScene.h"
#include <iostream>
#include <Log.h>
#include <FileManager.h>
using namespace std;

MainScene::MainScene(string config_path, int update_delay) : Scene(update_delay) {
    Log::PrintDebugLog("MainScene", "Constructor", "생성자 호출");
	this->zoom = 1.0;
	this->velocity = 0.1;
    this->simulator = new Simulator(config_path);
}

MainScene::~MainScene() {
    Log::PrintDebugLog("MainScene", "Destructor", "소멸자 호출");
    this->run = false;
    while (!this->process_completed);
    delete this->simulator;
}

int MainScene::Rendering() {
	if(this->simulator) this->simulator->Rendering(this->win, this->zoom);
    return 0;
}

int MainScene::EventProcess(Event::Event& evt) {
	switch (evt.type) {
	case Event::MOUSE_UP:
		this->simulator->OccurEarthquake(Coordinate(evt.mouse_data.x / this->zoom, evt.mouse_data.y / this->zoom));
		break;
	case Event::MOUSE_WHEEL:
		this->zoom *= sqrt(pow(2, evt.wheel_delta));
		if (this->zoom < 0) {
			this->zoom = 0;
		}
		break;
	case Event::KEY_DOWN:
		if (evt.keycode == SDLK_LEFT) {
			this->velocity *= 0.9;
		}
		else if(evt.keycode == SDLK_RIGHT) {
			this->velocity *= 1.1;
		}
		break;
	}
	return 0;
}

int MainScene::NormalProcess() {
	if(this->simulator) this->simulator->Update((double)this->delta_time * velocity * 0.5);
	return 0;
}
