#include "Event.h"

Event::Event::Event(EventType et) {
	this->type = et;
}

Event::Event::Event(EventType et, SDL_Keycode kc) {
	this->type = et;
	this->keycode = kc;
}

Event::Event::Event(EventType et, MouseData md) {
	this->type = et;
	this->mouse_data = md;
}

Event::Event::Event(EventType et, double wd) {
	this->type = et;
	this->wheel_delta = wd;
}
