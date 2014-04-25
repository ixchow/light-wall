#include <string.h>
#include "Patterns.h"
#include <memory.h>

Pattern::~Pattern() { }

class P_Xor : public Pattern {
public:
	P_Xor() : t(rand()) { }
	uint32_t t;
	virtual void draw(uint8_t *buffer) {
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
	virtual void draw(uint8_t *buffer) {
		for (uint32_t i = 0; i < LedsX * LedsY; ++i) {
			uint8_t val = rand();
			*(buffer++) = val;
			*(buffer++) = val;
			*(buffer++) = val;
		}
	}
};

class RampPattern : public Pattern {
public:
	uint8_t t;
	RampPattern(uint8_t *_grid, RampPtr _ramp = NULL) : t(rand()), grid(_grid), ramp(_ramp) {
		if (ramp == NULL) {
			ramp = all_ramps[rand() % RampCount];
		}
	}
	virtual void draw(uint8_t *buffer) {
		for (uint32_t i = 0; i < LedsX * LedsY; ++i) {
			read_ramp(ramp, grid[i] + t, reinterpret_cast< Px * >(&(buffer[i*3])));
		}
		++t;
	}
	uint8_t *grid;
	RampPtr ramp;
};

class P_Circle : public RampPattern {
public:
	P_Circle() : RampPattern(&grid[0]) {
		for (int16_t y = 0; y < (int16_t)LedsY; ++y) {
			for (int16_t x = 0; x < (int16_t)LedsX; ++x) {
				int16_t dis = (2 * x + 1 - LedsX) * (2 * x + 1 - LedsX) * PerJugY * PerJugY
				            + (2 * y + 1 - LedsY) * (2 * y + 1 - LedsY) * PerJugX * PerJugX;
				grid[y * LedsX + x] = dis / 8;
			}
		}
	}
	uint8_t grid[LedsX * LedsY];
};

class P_RandomWalk : public RampPattern {
public:
	P_RandomWalk() : RampPattern(&grid[0]) {
		memset(grid, 0, LedsX * LedsY);
		s = 0;
		x = rand() % JugsX;
		y = rand() % JugsY;
	}
	
	void step() {
		if (s == 0) {
			uint8_t dir = rand() % 8;
			if      (dir == 0) { dx = 1; dy = 0; }
			else if (dir == 1) { dx = 1; dy = 1; }
			else if (dir == 2) { dx = 0; dy = 1; }
			else if (dir == 3) { dx =-1; dy = 1; }
			else if (dir == 4) { dx =-1; dy = 0; }
			else if (dir == 5) { dx =-1; dy =-1; }
			else if (dir == 6) { dx = 0; dy =-1; }
			else /* dir == 7 */{ dx = 1; dy =-1; }
			s = rand() % 2 + 1;
		}
		uint8_t px = x;
		uint8_t py = y;
		x += dx;
		y += dy;
		if (x < 0) x += JugsX;
		if (x >= int8_t(JugsX)) x -= JugsX;
		if (y < 0) y += JugsY;
		if (y >= int8_t(JugsY)) y -= JugsY;
		for (uint8_t oy = 0; oy < PerJugY; ++oy) {
			for (uint8_t ox = 0; ox < PerJugX; ++ox) {
				grid[(y * PerJugY + oy) * LedsX + x * PerJugX + ox] += 16;
				grid[(y * PerJugY + oy) * LedsX + (JugsX - 1 - x) * PerJugX + ox] += 16;
				grid[(py * PerJugY + oy) * LedsX + px * PerJugX + ox] -= 7;
				grid[(py * PerJugY + oy) * LedsX + (JugsX - 1 - px) * PerJugX + ox] -= 7;
			}
		}
		--s;
	}
	virtual void draw(uint8_t *buffer) {
		step();
		t = 0;
		RampPattern::draw(buffer);
	}
	uint8_t grid[LedsX * LedsY];
	int8_t x, y, dx, dy, s;
};


class P_Life : public Pattern {
public:
	P_Life()
	{
		for (uint32_t x = 0; x < LedsX; x++) {
			for (uint32_t y = 0; y < LedsY; y++) {
				cells[x][y] = (rand() % 2 == 0);
			}
		}
		frameCount = 0;
	}

	virtual void draw(uint8_t *buffer) {
		frameCount = (frameCount + 1) % 5;
		if (frameCount != 4)
			return;
		for (uint32_t y = 0; y < LedsY; y++) {
			for (uint32_t x = 0; x < LedsX; x++) {
				uint8_t val = (cells[x][y] == true) ? 255 : 0;
				*(buffer++) = val;
				*(buffer++) = val;
				*(buffer++) = val;	
			}
		}

		// Calculate the next generation
		for (uint32_t y = 0; y < LedsY; y++) {
			for (uint32_t x = 0; x < LedsX; x++) {
				uint8_t neighbors = 0;
				if (x > 0 && cells[x-1][y])
					neighbors++;
				if (x < LedsX-1 && cells[x+1][y])
					neighbors++;
				if (y > 0 && cells[x][y-1])
					neighbors++;
				if (y < LedsY-1 && cells[x][y+1])
					neighbors++;
				if (x > 0 && y > 0 && cells[x-1][y-1])
					neighbors++;
				if (x < LedsX-1 && y > 0 && cells[x+1][y-1])
					neighbors++;
				if (x > 0 && y < LedsY-1 && cells[x-1][y+1])
					neighbors++;
				if (x < LedsX-1 && y < LedsY-1 && cells[x+1][y+1])
					neighbors++;

				if (neighbors < 2)
					next[x][y] = false;
				else if (cells[x][y] && (neighbors == 2 || neighbors == 3))
					next[x][y] = true;
				else if (cells[x][y] && (neighbors > 3))
					next[x][y] = false;
				else if (!cells[x][y] && (neighbors == 3))
					next[x][y] = true;
				else
					next[x][y] = cells[x][y];
			}
		}

		memcpy(cells, next, sizeof(bool) * LedsX * LedsY);
	}

	uint8_t frameCount;
	bool cells[LedsX][LedsY];
	bool next[LedsX][LedsY];
};

//----------------------------------------------------

template< typename P >
Pattern *create() { return new P(); }

CreatePattern all_patterns[PatternCount] = {
	&create< P_Circle >,
	&create< P_Xor >,
	&create< P_Rand >,
	&create< P_RandomWalk >,
	&create< P_Life >
};
