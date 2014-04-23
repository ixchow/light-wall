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
	x = x * 13 + ((s->t * 2) % 256);
	read_ramp(s->ramp1, x, px);
}

//----------------------------------------------------

void No_Init(PatternState *) {
}

void No_Update(PatternState *) {
}

PatternInfo all_patterns[PatternCount] = {
	{ &No_Init, &No_Update, &P_Checker_Read },
	{ &No_Init, &No_Update, &P_Zags_Read },
};
