#include "PatternMixer.h"
#include "Patterns.h"

PatternMixer::PatternMixer() : pattern(NULL), ticks_to_remix(0) {
	next();
}
PatternMixer::~PatternMixer() {
	if (pattern) {
		delete pattern;
		pattern = NULL;
	}
}

void PatternMixer::draw(uint8_t *led_buffer) {
	if (ticks_to_remix) {
		--ticks_to_remix;
		pattern->draw(led_buffer);
	} else {
		next();
	}
}

void PatternMixer::next() {
	if (pattern) {
		delete pattern;
		pattern = NULL;
	}
	pattern = all_patterns[rand() % PatternCount]();
	ticks_to_remix = TicksPerSecond * 5 + (rand() % (TicksPerSecond * 5));
}
