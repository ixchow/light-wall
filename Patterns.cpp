#include "Patterns.h"

//----------------------------------------------------

extern void P_Checker_Read(PatternState *, uint16_t x, uint16_t y, Px *);

//----------------------------------------------------

void P_Zags_Read(PatternState *s, uint16_t x, uint16_t y, Px *px) {
	if (y > LedsY / 2) {
		x += (LedsY - y) * (PerJugX / PerJugY);
	} else {
		x += y * (PerJugX / PerJugY);
	}
	x *= (s->p1 & 0xf) + 12;
	if (s->p1 & 0x10) {
		x += ((s->t * 2) % 256);
	} else {
		x += 255 - ((s->t * 2) % 256);
	}
	read_ramp(s->ramp1, x, px);
}

//----------------------------------------------------

void Grid_Circle_Init(PatternState *s) {
	for (int16_t y = 0; y < LedsY; ++y) {
		for (int16_t x = 0; x < LedsX; ++x) {
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

void No_Init(PatternState *) {
}

void No_Update(PatternState *) {
}

PatternInfo all_patterns[PatternCount] = {
	{ &Grid_Circle_Init, &No_Update, &Grid_Read },
	{ &No_Init, &No_Update, &P_Checker_Read },
	{ &No_Init, &No_Update, &P_Zags_Read },
};
