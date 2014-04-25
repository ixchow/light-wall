#include <OctoWS2811.h>
#include "LedMap.h"
#include "Patterns.h"
#include "PatternMixer.h"

const int ledsPerStrip = 100;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_RGB | WS2811_800kHz;

const int ledPin = 13;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

uint8_t led_buffer[LedsX * LedsY * 3];
PatternMixer *pm = NULL;

volatile int ledVal;
volatile int frameFlag;

void blink_led(void) {
  frameFlag = 1;
  digitalWrite(ledPin, ledVal ^= 1);
}

IntervalTimer frameTimer;

void setup() {
  pinMode(ledPin, OUTPUT);
  frameFlag = 1;
  frameTimer.begin(blink_led, 1000000 / TicksPerSecond);
  leds.begin();
  for (int i = 0; i < leds.numPixels(); ++i) {
    leds.setPixel(i, 0xffffff);
  }
  leds.show();
  pm = new PatternMixer();
}


void loop() {
  pm->draw(led_buffer);
  for (int i = 0; i < LedsX * LedsY; ++i) {
    leds.setPixel(LedMap[i], (int(led_buffer[3*i+0]) << 16) | (int(led_buffer[3*i+1]) << 8) | led_buffer[3*i+2]);
  }
  while (frameFlag == 0) {
      /* busy waiting */
  }
  frameFlag = 0;
  leds.show();
}
