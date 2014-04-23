#pragma once

struct Px;

//Ramps map values [0-255] into rgb color data, and are represented
// by an opaque data blob, at the moment:

struct RampData;
void read_ramp(RampData const *data, uint8_t v, Px *px);


const uint32_t RampCount = 2;
extern RampData *all_ramps[RampCount];
