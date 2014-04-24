#pragma once

#include <stdint.h>
#include <cstdlib>

#include "Ramps.h"

const uint32_t PerJugX = 2;
const uint32_t PerJugY = 1;
const uint32_t JugsX = 24;
const uint32_t JugsY = 12;
const uint32_t LedsX = PerJugX * JugsX;
const uint32_t LedsY = PerJugY * JugsY;

class Pattern {
public:
	//buffer is rgbrgbrgb...
	virtual ~Pattern();
	virtual void draw(uint8_t *buffer) = 0;
};

typedef Pattern *(*CreatePattern)();

const uint32_t PatternCount = 3;
extern CreatePattern all_patterns[PatternCount];
