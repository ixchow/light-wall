#include "PatternMixer.h"
#include "Patterns.h"

PatternMixer::PatternMixer() : pattern(NULL), ticks_to_remix(0), pattern_order(NULL), next_in_order(0) {
	pattern_order = new uint8_t[PatternCount];
	for (uint8_t i = 0; i < PatternCount; ++i) {
		pattern_order[i] = i;
	}
	next_in_order = PatternCount;
	next();
}

PatternMixer::~PatternMixer() {
	if (pattern) {
		delete pattern;
		pattern = NULL;
	}

	delete pattern_order;
	pattern_order = NULL;
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
	if (next_in_order >= PatternCount) {
		next_in_order = 0;
		for (uint8_t i = 0; i < PatternCount; ++i) {
			uint8_t &sel = pattern_order[i + (rand() % (PatternCount - i))];
			uint8_t temp = sel;
			sel = pattern_order[i];
			pattern_order[i] = temp;
		}
	}
	pattern = all_patterns[pattern_order[next_in_order]]();
	uint32_t rv = rand();
	//if (rv & 0x1) {
	//	ticks_to_remix = TicksPerSecond / 2 + (rv % TicksPerSecond);
	//} else {
		ticks_to_remix = TicksPerSecond * 30 + (rv % (TicksPerSecond * 5));
	//}
	++next_in_order;
}
