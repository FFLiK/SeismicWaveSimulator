#pragma once
#include <Color.h>
#include <map>

namespace ColorTable {
	enum ColorType {LAYER, P_WAVE, S_WAVE};
	extern const std::map<ColorType, Color::RGB> value;
}