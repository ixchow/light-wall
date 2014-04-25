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
const uint32_t TicksPerSecond = 30;

class Pattern {
public:
	//buffer is rgbrgbrgb...
	virtual ~Pattern();
	virtual void draw(uint8_t *buffer) = 0;
};

typedef Pattern *(*CreatePattern)();

const uint32_t PatternCount = 7;

extern CreatePattern all_patterns[PatternCount];


class WanderingDiamonds : public Pattern {
private:
    static const int seedCount = 5;
    int p[seedCount];
    int c[seedCount];

public:
    WanderingDiamonds();
    virtual void draw(uint8_t * buffer); 
};
		
