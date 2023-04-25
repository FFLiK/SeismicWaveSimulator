#pragma once
#include <SDL.h>
#include <Event.h>
#include <queue>
#include <Window.h>
using namespace std;

class Scene {
public:
	Scene(int update_delay);
	virtual ~Scene();

	virtual int Rendering() abstract;
	int SetWindow(Window* win);
	int PushEvent(Event::Event evt);
	bool IsRun();

	int __Process__();

protected:
	bool run = false;
	bool process_completed = false;

	Window* win;

	queue<Event::Event> event_queue;

	virtual int EventProcess(Event::Event &evt) abstract;
	virtual int NormalProcess() abstract;

	int update_delay;
};