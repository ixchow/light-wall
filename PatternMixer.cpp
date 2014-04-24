#include "PatternMixer.h"
#include "Patterns.h"

PatternMixer::PatternMixer() : pattern(NULL) {
	next();
}
PatternMixer::~PatternMixer() {
	if (pattern) {
		delete pattern;
		pattern = NULL;
	}
}

void PatternMixer::draw(uint8_t *led_buffer) {
	pattern->draw(led_buffer);
}

void PatternMixer::next() {
	if (pattern) {
		delete pattern;
		pattern = NULL;
	}
	pattern = all_patterns[rand() % PatternCount]();
}
