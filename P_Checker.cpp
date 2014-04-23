#include "Patterns.h"

void P_Checker_Read(PatternState *, uint16_t x, uint16_t y, Px *px) {
	if (((x / PerJugX) ^ (y / PerJugY)) & 0x1) {
		px->r = 0xff;
		px->g = 0xff;
		px->b = 0xff;
	} else {
		px->r = 0x0;
		px->g = 0x0;
		px->b = 0x0;
	}
};
