#pragma once

#include <stdint.h>

const uint32_t PerJugX = 2;
const uint32_t PerJugY = 1;
const uint32_t JugsX = 12;
const uint32_t JugsY = 12;
const uint32_t LedsX = PerJugX * JugsX;
const uint32_t LedsY = PerJugY * JugsY;

struct Px {
	uint8_t r,g,b;
};

typedef void (*RampFn)(void *, uint8_t v, Px *px);

struct PatternState {
	uint32_t t;
	RampFn ramp1;
	void *ramp_param_1;

	uint8_t grid[LedsX * LedsY * 3];
};

typedef void (*InitFn)(PatternState *);
typedef void (*PatternFn)(PatternState *, uint16_t x, uint16_t y, Px *px);
