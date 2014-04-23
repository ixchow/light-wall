#pragma once

#include <stdint.h>

#include "Ramps.h"

const uint32_t PerJugX = 2;
const uint32_t PerJugY = 1;
const uint32_t JugsX = 12;
const uint32_t JugsY = 12;
const uint32_t LedsX = PerJugX * JugsX;
const uint32_t LedsY = PerJugY * JugsY;

struct Px {
	uint8_t r,g,b;
};

struct PatternState {
	//these are maintained by the pattern driver:
	uint32_t t; //tick counter, increases by one each frame
	RampData *ramp1; //a randomly chosen color ramp
	uint16_t p1; //"parameter 1", randomized to 0 - 0xffff

	//-------------------------
	//These are misc. data fields; if your pattern uses them,
	// make sure to init and update them:
	uint8_t grid[LedsX * LedsY * 3];
};

struct PatternInfo {
	void (*init)(PatternState *);
	void (*update)(PatternState *);
	void (*read)(PatternState *, uint16_t x, uint16_t y, Px *px);
};

const uint32_t PatternCount = 2;
extern PatternInfo all_patterns[PatternCount];
