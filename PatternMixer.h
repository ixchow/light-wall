#pragma once

#include "Patterns.h"

class PatternMixer {
public:
	PatternMixer();
	~PatternMixer();

	void draw(uint8_t *led_buffer);

	void next(); //advance to next pattern

	Pattern *pattern;
	uint32_t ticks_to_remix;

	uint8_t *pattern_order;
	uint8_t next_in_order;
private:
	PatternMixer(PatternMixer const &); //do not use
};
