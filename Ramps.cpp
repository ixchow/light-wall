#include "Patterns.h"
#include "Ramps.h"


//-----------------------------------------------

void R_Grey(uint8_t v, Px *px) {
	px->r = v;
	px->g = v;
	px->b = v;
}

//-----------------------------------------------

void R_Rainbow(uint8_t v, Px *px) {
	if (v < 256 / 6) {
		v -= 256 * 0 / 6;
		v = v * 6;
		px->r = 0xff;
		px->g = 0x00 + v;
		px->b = 0x00;
	} else if (v < 256 * 2 / 6) {
		v -= 256 * 1 / 6;
		v = v * 6;
		px->r = 0xff - v;
		px->g = 0xff;
		px->b = 0x00;
	} else if (v < 256 * 3 / 6) {
		v -= 256 * 2 / 6;
		v = v * 6;
		px->r = 0x00;
		px->g = 0xff;
		px->b = 0x00 + v;
	} else if (v < 256 * 4 / 6) {
		v -= 256 * 3 / 6;
		v = v * 6;
		px->r = 0x00;
		px->g = 0xff - v;
		px->b = 0xff;
	} else if (v < 256 * 5 / 6) {
		v -= 256 * 4 / 6;
		v = v * 6;
		px->r = 0x00 + v;
		px->g = 0x00;
		px->b = 0xff;
	} else { //v < 256 * 6 / 6, of course
		v -= 256 * 5 / 6;
		v = v * 6;
		px->r = 0xff;
		px->g = 0x00;
		px->b = 0xff - v;
	}
}

//-----------------------------------------------

struct RampData {
	void (*func)(uint8_t v, Px *px);
};

void read_ramp(RampData const *data, uint8_t v, Px *px) {
	data->func(v, px);
}

RampData GreyData = {&R_Grey};
RampData RainbowData = {&R_Rainbow};

RampData *all_ramps[RampCount] = {
	&GreyData,
	&RainbowData,
};
