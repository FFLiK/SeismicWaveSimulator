#include "MainScene.h"
#include <iostream>
#include <Log.h>
#include <FileManager.h>
using namespace std;

MainScene::MainScene(string config_path, int update_delay) : Scene(update_delay) {
    Log::PrintDebugLog("MainScene", "Constructor", "생성자 호출");
    this->simulator = new Simulator(config_path);
	this->zoom = 1.0;
}

MainScene::~MainScene() {
    Log::PrintDebugLog("MainScene", "Destructor", "소멸자 호출");
    this->run = false;
    while (!this->process_completed);
    //TODO : Delete Something
    delete this->simulator;
}

int MainScene::Rendering() {
	this->simulator->Rendering(this->ren, this->zoom);
	
    return 0;
}

int MainScene::EventProcess(Event& evt) {
	this->simulator->Update();
	return 0;
}

int MainScene::NormalProcess() {
    return 0;
}
