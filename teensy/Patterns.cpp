#include <string.h>
#include "Patterns.h"

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

class P_Shimmer : public Pattern {
public:
	virtual void draw(uint8_t *buffer) {
		for (uint32_t i = 0; i + 1 < LedsX * LedsY; i += 2) {
			uint32_t rv = rand();
			uint8_t r = 128 - 16 + (rv & 0x1f);
			uint8_t g = 128 - 16 + ((rv >> 5) & 0x1f);
			uint8_t b = 128 - 16 + ((rv >> 10) & 0x1f);
			*(buffer++) = r;
			*(buffer++) = g;
			*(buffer++) = b;
			*(buffer++) = 255 - r;
			*(buffer++) = 255 - g;
			*(buffer++) = 255 - b;
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
		speed = rand();
		if (speed & 0x10) {
			speed = -2;
		} else {
			speed = 2;
		}
		speed -= 1;
	}
	virtual void draw(uint8_t *buffer) {
		RampPattern::draw(buffer);
		t += speed;
	}
	int8_t speed;
	uint8_t grid[LedsX * LedsY];
};

class P_RandomWalk : public RampPattern {
public:
	P_RandomWalk() : RampPattern(&grid[0]) {
		memset(grid, rand(), LedsX * LedsY);
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
			uint8_t rv = rand();
			s = 1 + (rv & 1) + ((rv >> 1) & 1) + ((rv >> 2) & 1);
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
				grid[(y * PerJugY + oy) * LedsX + x * PerJugX + ox] += 8;
				grid[(y * PerJugY + oy) * LedsX + (JugsX - 1 - x) * PerJugX + ox] += 8;
				grid[(py * PerJugY + oy) * LedsX + px * PerJugX + ox] += 7;
				grid[(py * PerJugY + oy) * LedsX + (JugsX - 1 - px) * PerJugX + ox] += 7;
			}
		}
		--s;
	}
	virtual void draw(uint8_t *buffer) {
		step();
		RampPattern::draw(buffer);
	}
	uint8_t grid[LedsX * LedsY];
	int8_t x, y, dx, dy, s;
};

class Drip {
public:
	Drip() {
		y = -1;
		s = 0;
		lim = rand() % TicksPerSecond;
	}
	void step(RampPtr ramp, uint8_t *buffer) {
		if (s >= lim) {
			x = rand() % JugsX;
			y = rand() % (JugsY - 1) + 1;
			s = -int8_t(TicksPerSecond)/4;
			lim = (TicksPerSecond + (rand() % TicksPerSecond)) / 2;
			uint8_t v = rand();
			read_ramp(ramp, v, &c);
		}
		if (y >= 0) {
			uint8_t r, g, b;
			if (s < 0) {
				r = g = b = 255;
			} else {
				r = c.r; g = c.g; b = c.b;
				const uint8_t sub = 14;
				if (c.r > sub) c.r -= sub;
				else c.r = 0;
				if (c.g > sub) c.g -= sub;
				else c.g = 0;
				if (c.b > sub) c.b -= sub;
				else c.b = 0;
			}
			for (uint8_t oy = 0; oy < PerJugY; ++oy) {
				for (uint8_t ox = 0; ox < PerJugX; ++ox) {
					buffer[((y * PerJugY + oy) * LedsX + x * PerJugX + ox) * 3 + 0] = r;
					buffer[((y * PerJugY + oy) * LedsX + x * PerJugX + ox) * 3 + 1] = g;
					buffer[((y * PerJugY + oy) * LedsX + x * PerJugX + ox) * 3 + 2] = b;

/*
					buffer[(((JugsY - 1 - y) * PerJugY + oy) * LedsX + (LedsX - 1 - (x + ox))) * 3 + 0] = r;
					buffer[(((JugsY - 1 - y) * PerJugY + oy) * LedsX + (LedsX - 1 - (x + ox))) * 3 + 1] = g;
					buffer[(((JugsY - 1 - y) * PerJugY + oy) * LedsX + (LedsX - 1 - (x + ox))) * 3 + 2] = b;*/
				}
			}
			if (s >= 0) {
				--y;
			}
		}
		++s;
	}
	int8_t x, y, s, lim;
	Px c;
};

class P_Drip : public Pattern {
public:
	Drip drips[7];
	RampPtr ramp;
	bool first;

	P_Drip() {
		first = true;
		ramp = all_ramps[rand() % RampCount];
	}
	
	virtual void draw(uint8_t *buffer) {
		if (first) {
			memset(buffer, 0, LedsX * LedsY * 3);
			first = false;
		}
		for (uint32_t i = 0; i < 7; ++i) {
			drips[i].step(ramp, buffer);
		}
	}
};

class P_Life : public Pattern {
public:
	P_Life()
	{
		for (uint32_t x = 0; x < JugsX; x++) {
			for (uint32_t y = 0; y < JugsY; y++) {
				cells[x][y] = (rand() % 2 != 0);
			}
		}
		frameCount = 0;
		generation = 0;
		black.r = black.g = black.b = 0;
	}

	void drawPixel(uint8_t* buffer, uint32_t x, uint32_t y, Px& color)
	{
		buffer[(y * LedsX + x * PerJugX) * 3 + 0] = color.r;
		buffer[(y * LedsX + x * PerJugX) * 3 + 1] = color.g;
		buffer[(y * LedsX + x * PerJugX) * 3 + 2] = color.b;
		buffer[(y * LedsX + x * PerJugX + 1) * 3 + 0] = color.r;
		buffer[(y * LedsX + x * PerJugX + 1) * 3 + 1] = color.g;
		buffer[(y * LedsX + x * PerJugX + 1) * 3 + 2] = color.b;
	}

	void darken(uint8_t* buffer, uint32_t x, uint32_t y)
	{
		Px color;
		color.r = buffer[(y * LedsX + x * PerJugX) * 3 + 0];
		color.g = buffer[(y * LedsX + x * PerJugX) * 3 + 1];
		color.b = buffer[(y * LedsX + x * PerJugX) * 3 + 2];
		color.r = (color.r > DARKEN_BY) ? color.r - DARKEN_BY : 0;
		color.g = (color.g > DARKEN_BY) ? color.g - DARKEN_BY : 0;
		color.b = (color.b > DARKEN_BY) ? color.b - DARKEN_BY : 0;
		drawPixel(buffer, x, y, color);
	}


	virtual void draw(uint8_t *buffer) {
		frameCount = (frameCount + 1) % 5;
		if (frameCount != 4)
			return;
		
		// Draw out the first generation in full
		Px generationColor;
		read_ramp(all_ramps[3], generation % 256, &generationColor);
		if (generation == 0) {
			for (uint32_t y = 0; y < JugsY; y++) {
				for (uint32_t x = 0; x < JugsX; x++) {
					drawPixel(buffer, x, y, cells[x][y] ? generationColor : black);
				}
			}
		}

		// Calculate the next generation
		for (uint32_t y = 0; y < JugsY; y++) {
			for (uint32_t x = 0; x < JugsX; x++) {
				uint8_t neighbors = 0;
				if (x > 0 && cells[x-1][y])
					neighbors++;
				if (x < JugsX-1 && cells[x+1][y])
					neighbors++;
				if (y > 0 && cells[x][y-1])
					neighbors++;
				if (y < JugsY-1 && cells[x][y+1])
					neighbors++;
				if (x > 0 && y > 0 && cells[x-1][y-1])
					neighbors++;
				if (x < JugsX-1 && y > 0 && cells[x+1][y-1])
					neighbors++;
				if (x > 0 && y < JugsY-1 && cells[x-1][y+1])
					neighbors++;
				if (x < JugsX-1 && y < JugsY-1 && cells[x+1][y+1])
					neighbors++;

				if (neighbors < 1) {
					next[x][y] = false;
					darken(buffer, x, y);
				} else if (cells[x][y] && (neighbors > 3)) {
					next[x][y] = false;
					darken(buffer, x, y);
				} else if (!cells[x][y] && (neighbors == 3)) {
					next[x][y] = true;
					drawPixel(buffer, x, y, generationColor);
				} else if (!cells[x][y]) {
					next[x][y] = cells[x][y];
					darken(buffer, x, y);
				} else
					next[x][y] = cells[x][y];
			}
		}

		memcpy(cells, next, sizeof(bool) * JugsX * JugsY);
		generation += 2;
	}

	uint8_t frameCount;
	bool cells[JugsX][JugsY];
	bool next[JugsX][JugsY];
	uint32_t generation;
	Px black;
	static const uint8_t DARKEN_BY = 50;
};

//----------------------------------------------------

template< typename P >
Pattern *create() { return new P(); }

CreatePattern all_patterns[PatternCount] = {
	&create< P_Circle >,
	//&create< P_Xor >,
	&create< WanderingDiamonds >,
	&create< P_Shimmer >,
	&create< P_RandomWalk >,
	&create< P_Drip >,
	&create< P_Life >,

};
