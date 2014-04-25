#pragma once

struct Px;

//Ramps map values [0-255] into rgb color data, and are represented
// by an opaque data blob, at the moment:

struct Px {
	uint8_t r,g,b;
};

typedef void * RampPtr;
inline void read_ramp(RampPtr _data, uint8_t v, Px *px) {
	uint8_t *data = reinterpret_cast< uint8_t * >(_data);
	px->r = data[3*v+0];
	px->g = data[3*v+1];
	px->b = data[3*v+2];
}


const uint32_t RampCount = 2;
extern RampPtr all_ramps[RampCount];
