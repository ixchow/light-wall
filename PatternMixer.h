#pragma once

#include "Patterns.h"

class PatternMixer {
public:
	PatternMixer();
	~PatternMixer();

	void draw(uint8_t *led_buffer);

	void next(); //advance to next pattern

	Pattern *pattern;
	uint8_t ticks_to_remix;
};
