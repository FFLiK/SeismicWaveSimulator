#pragma once
#include <Scene.h>
#include <Simulator.h>

#define MAINSCENE(x) ((MainScene*)x)

class MainScene : public Scene {
public:
	MainScene(string config_path, int update_delay = 0);
	~MainScene();
	int Rendering();

private:
	int EventProcess(Event& evt);
	int NormalProcess();

	Simulator* simulator;
	double zoom;
};

