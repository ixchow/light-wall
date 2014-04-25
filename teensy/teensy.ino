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

void setup() {
  leds.begin();
  for (int i = 0; i < leds.numPixels(); ++i) {
    leds.setPixel(i, 0xffffff);
  }
  leds.show();
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < LedsX * LedsY; ++i) {
    led_buffer[3*i + 0] = rand();
    led_buffer[3*i + 1] = rand();
    led_buffer[3*i + 2] = rand();
  }
  pm = new PatternMixer();
}


void loop() {
  digitalWrite(ledPin, HIGH);
  pm->draw(led_buffer);
  digitalWrite(ledPin, LOW);
  for (int i = 0; i < LedsX * LedsY; ++i) {
    leds.setPixel(LedMap[i], (int(led_buffer[3*i+0]) << 16) | (int(led_buffer[3*i+1]) << 8) | led_buffer[3*i+2]);
  }
  leds.show();
  delay(1000 / TicksPerSecond);
}
