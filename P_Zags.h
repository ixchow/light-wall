#include "Pattern.h"

static void ZagsFn(PatternState *s, uint16_t x, uint16_t y, Px *px) {
	if (y > LedsY / 2) {
		x += (LedsY - y) * (PerJugX / PerJugY);
	} else {
		x += y * (PerJugX / PerJugY);
	}
	x = x * 13 + ((s->t * 2) % 256);
	s->ramp1(s->ramp_param_1, x, px);
}
