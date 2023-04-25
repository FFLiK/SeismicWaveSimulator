#pragma once
#include <SDL.h>

namespace Event {
	enum EventType { NONE, QUIT, KEY_DOWN, KEY_UP, MOUSE_DONW, MOUSE_UP, MOUSE_WHEEL };
	enum Mousecode { LEFT, RIGHT, MIDDLE };
	class MouseData {
	public:
		Mousecode mouse_code;
		int x, y;
	};

	class Event {
	public:
		EventType type;
		SDL_Keycode keycode;
		MouseData mouse_data;
		double wheel_delta;

		Event(EventType et);
		Event(EventType et, SDL_Keycode kc);
		Event(EventType et, MouseData md);
		Event(EventType et, double wd);
	};
};