#include "Patterns.h"

#ifndef PROGMEM
#define PROGMEM
#endif

#define COLOR_WHEEL_COUNT 60
const static uint8_t colorWheel[COLOR_WHEEL_COUNT][3] PROGMEM = {
    {0,255,0}, {20,255,0}, {40,252,0}, {60,249,0}, {79,243,0}, 
    {98,237,0}, {116,228,0}, {134,218,0}, {150,207,0}, {166,195,0}, 
    {181,181,0}, {195,166,0}, {207,150,0}, {218,134,0}, {228,116,0}, 
    {237,98,0}, {243,79,0}, {249,60,0}, {252,40,0}, {255,20,0}, 
    {255,0,0}, {255,0,20}, {252,0,40}, {249,0,60}, {243,0,79}, 
    {237,0,98}, {228,0,116}, {218,0,134}, {207,0,150}, {195,0,166}, 
    {181,0,181}, {166,0,195}, {150,0,207}, {134,0,218}, {116,0,228}, 
    {98,0,237}, {79,0,243}, {60,0,249}, {40,0,252}, {20,0,255}, 
    {0,0,255}, {0,20,255}, {0,40,252}, {0,60,249}, {0,79,243}, 
    {0,98,237}, {0,116,228}, {0,134,218}, {0,150,207}, {0,166,195}, 
    {0,181,181}, {0,195,166}, {0,207,150}, {0,218,134}, {0,228,116}, 
    {0,237,98}, {0,243,79}, {0,249,60}, {0,252,40}, {0,255,20} 
};

class WheelColor {
    int pos;
    int s;

    static int normalize(int val) {
	val %= COLOR_WHEEL_COUNT;
	if (val < 0)
	    val += COLOR_WHEEL_COUNT;
	return val;
    }
	    

public:
    WheelColor(int wheelPos = 0, int skip = 1) {
	pos = normalize(wheelPos);
	s = normalize(skip);
    }

    void nextPos(int skip = 0) {
	if (skip == 0)
	    pos += s;
	else
	    pos += skip;

	pos = normalize(pos);
    }
	
    uint8_t r() {
	return colorWheel[pos][0];
    }

    uint8_t g() {
	return colorWheel[pos][1];
    }
	
    uint8_t b() {
	return colorWheel[pos][2];
    }

    const uint8_t* rgb() {
	return colorWheel[pos];
    }

    int pack() {
	return s * COLOR_WHEEL_COUNT + pos;
    }
};

WheelColor unpackWheelColor(int packedVal) {
    int pos = packedVal % COLOR_WHEEL_COUNT;
    int s = packedVal / COLOR_WHEEL_COUNT;
    return WheelColor(pos, s);
}

class QueueArray {  // also a stack
  int s;
  int head;
  int tail;
  
 public:
  QueueArray(int size) {
    s = size;
    head = 0;
    tail = 0;
  }
  
  int len() {
    if (tail >= head)
      return tail - head;
    return tail + s - head;
  }
  
  int push() {  // push to tail of queue
    int ret = tail++;
    if (tail == s)
      tail = 0;
    return ret;
  }
  
  int pull() {  // pull from head of queue
    int ret = head++;
    if (head == s)
      head = 0;
    return ret;
  }
  
  int pop() {  // pop off tail of queue
    if (--tail < 0)
      tail = s-1;
    return tail;
  }
};

struct IntPair {
  int a;
  int b;

  IntPair() {
  }
  
  IntPair(int _a, int _b) {
    a = _a;
    b = _b;
  }
};

class PixelSet {
    static const int PIXEL_SET_CHARS = ((JugsX * JugsY + 7) / 8);
    unsigned char pixelSet[PIXEL_SET_CHARS];

public:
  void clear() {
    for (int i  = 0; i < PIXEL_SET_CHARS; ++i) {
      pixelSet[i] = 0;
    }
  }

  void set(int p) {
    int b = p / 8;
    int bt = 1 << (p % 8);
    pixelSet[b] |= bt;
  }

  int setGet(int p) {
    int b = p / 8;
    int bt = 1 << (p % 8);
    int ret = pixelSet[b] & bt;
    if (!ret) {
      pixelSet[b] |= bt;
    }
    return ret;
  }

  int get(int p) {
    int b = p / 8;
    int bt = 1 << (p % 8);
    return pixelSet[b] & bt;
  }
};

PixelSet pixelSet;

class Plotter {
    uint8_t *buf;

public:
    Plotter(uint8_t *buffer) {
	buf = buffer;
    }

    void plot(int x, int y, const uint8_t *c) {
	if ((x >= (int)JugsX) || (x < 0) || (y >= (int)JugsY) || (y < 0)) {
		int badOffset = 500000000;
		buf[badOffset] = 10;
	    }
	int offset = y * PerJugY * LedsX * 3;
	offset += x * PerJugX * 3;
	for (unsigned int jy = 0; jy < PerJugY; ++jy) {
	    int xOff = 0;
	    for (unsigned int jx = 0; jx < PerJugX; ++jx) {
		int cOff = 0;
		buf[offset + xOff++] = c[cOff++];
		buf[offset + xOff++] = c[cOff++];
		buf[offset + xOff++] = c[cOff];
	    }
	    offset += LedsX * PerJugX * 3;
	}
    }

    void plot(int p, const uint8_t *c) {
	int x = p % JugsX;
	int y = p / JugsX;
	plot(x, y, c);
    }

    static int move(int p, int x, int y) {
	int px = p % JugsX;
	int py = p / JugsX;
	px += x;
	px %= JugsX;
	if (px < 0)
	    px += JugsX;
	py += y;
	py %= JugsY;
	if (py < 0)
	    py += JugsY;
	return px + py * JugsX;
    }
	
    static int up(int p) {
	return move(p, 0, 1);
    }

    static int down(int p) {
	return move(p, 0, -1);
    }

    static int right(int p) {
	return move(p, 1, 0);
    }

    static int left(int p) {
	return move(p, -1, 0);
    }
};
	    
#include <stdio.h>    

void diamondRainbowWash(Plotter &b, int count, int *origins, int *originColor) { 
    //FILE *f = fopen("debug", "w+");
    pixelSet.clear();
    IntPair q[300];
    QueueArray qa(300);
    
    for (int i; i < count; ++i)
	q[qa.push()] = IntPair(origins[i], originColor[i]);
    
    while (qa.len()) {
	IntPair pair = q[qa.pull()];
	int p = pair.a;

	//fprintf(f, "%d\n", p);
	//fflush(f);

	int c = pair.b;

	if (pixelSet.setGet(p))
	    continue;

	WheelColor wc = unpackWheelColor(c);
	b.plot(p, wc.rgb());
  
	wc.nextPos();
	c = wc.pack();
	
	q[qa.push()] = IntPair(b.up(p), c);
	q[qa.push()] = IntPair(b.down(p), c);
	q[qa.push()] = IntPair(b.left(p), c);
	q[qa.push()] = IntPair(b.right(p), c);
    }
    //fclose(f);
} 

WanderingDiamonds::WanderingDiamonds() {
    for (int i = 0; i < seedCount; ++i) {
	p[i] = rand() % (JugsX * JugsY);
	WheelColor wc = WheelColor(0, 6);
	c[i] = wc.pack();
    }
}
     

void WanderingDiamonds::draw(uint8_t * buffer) {
    Plotter b = Plotter(buffer);
    
    diamondRainbowWash(b, seedCount, p, c);
    
    for (int i = 0; i < seedCount; ++i) {
	WheelColor wc = unpackWheelColor(c[i]);
	wc.nextPos(1);
	c[i] = wc.pack();
	
	int d = rand() % 4;
	switch(d) {
	case 0:
	    p[i] = b.up(p[i]);
	    break;
	case 1:
	    p[i] = b.down(p[i]);
	    break;
	case 2:
	    p[i] = b.left(p[i]);
	    break;
	case 3:
	    p[i] = b.right(p[i]);
	    break;
	}
    }
}
		
