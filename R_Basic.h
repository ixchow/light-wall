#include "Pattern.h"

static void R_Grey(void *, uint8_t v, Px *px) {
	px->r = v;
	px->g = v;
	px->b = v;
}

static void R_Rainbow(void *, uint8_t v, Px *px) {
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

static void R_T_Peak(void *param, uint8_t v, Px *px) {
	if (v >= 128) {
		v = (255 - v) * 2 + 1;
	} else {
		v *= 2;
	}
	((RampFn)param)(NULL, v, px);
}
