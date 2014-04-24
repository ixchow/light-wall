#include "Patterns.h"

Pattern::~Pattern() { }

class P_Xor : public Pattern {
public:
	P_Xor() : t(0) { }
	uint32_t t;
	virtual void tick(uint8_t *buffer) {
		++t;
		for (uint32_t y = 0; y < LedsY; ++y) {
			for (uint32_t x = 0; x < LedsX; ++x) {
				uint8_t v = ((x >> 1) * 16) ^ (y * 16 + t);
				v += t >> 1;
				buffer[(y * LedsX + x) * 3 + 0] = v;
				buffer[(y * LedsX + x) * 3 + 1] = v;
				buffer[(y * LedsX + x) * 3 + 2] = v;
			}
		}
	}
};

class P_Rand : public Pattern {
public:
	virtual void tick(uint8_t *buffer) {
		for (uint32_t i = 0; i < LedsX * LedsY; ++i) {
			uint8_t val = rand();
			*(buffer++) = val;
			*(buffer++) = val;
			*(buffer++) = val;
		}
	}
};

/*
//TODO: port to new class-based patterns:
void Grid_Circle_Init(PatternState *s) {
	for (int16_t y = 0; y < (int16_t)LedsY; ++y) {
		for (int16_t x = 0; x < (int16_t)LedsX; ++x) {
			int16_t dis = (2 * x + 1 - LedsX) * (2 * x + 1 - LedsX) * PerJugY * PerJugY
			            + (2 * y + 1 - LedsY) * (2 * y + 1 - LedsY) * PerJugX * PerJugX;
			s->grid[y * LedsX + x] = dis / 8;
		}
	}
}

//----------------------------------------------------

void Grid_Read(PatternState *s, uint16_t x, uint16_t y, Px *px) {
	uint8_t v = s->grid[y * LedsX + x];
	v += s->t * (s->p1 & 0x7);
	if (s->p1 & 0x8) v = 255 - v;
	read_ramp(s->ramp1, v, px);
}

//----------------------------------------------------
*/

template< typename P >
Pattern *create() { return new P(); }

CreatePattern all_patterns[PatternCount] = {
	&create< P_Xor >,
	&create< P_Rand >,
	&create< P_Rand >,
};
