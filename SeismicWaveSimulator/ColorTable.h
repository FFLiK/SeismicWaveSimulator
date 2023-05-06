#pragma once
#include <Color.h>
#include <map>

namespace ColorTable {
	enum ColorType {LAYER, P_WAVE, S_WAVE, P_REFRACTED_WAVE, S_REFRACTED_WAVE, RECEIVER, GRAPH};
	extern const std::map<ColorType, Color::RGB> value;
}