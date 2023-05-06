#include <ColorTable.h>
namespace ColorTable {
	const std::map<ColorType, Color::RGB> value = {
		{LAYER, Color::RGB(150, 150, 150)},
		{P_WAVE, Color::RGB(145, 176, 255)},
		{S_WAVE, Color::RGB(255, 145, 182)},
		{P_REFRACTED_WAVE, Color::RGB(145, 176, 255)},
		{S_REFRACTED_WAVE, Color::RGB(255, 145, 182)},
		{RECEIVER, Color::RGB(145, 255, 211)},
		{HYPOCENTER, Color::RGB(255, 255, 255)}
	};
}