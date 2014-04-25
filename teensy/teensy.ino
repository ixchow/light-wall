#include <OctoWS2811.h>
#include "LedMap.h"
#include "Patterns.h"

const int ledsPerStrip = 100;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_RGB | WS2811_800kHz;

const int ledPin = 13;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
 leds.begin();
 for (int i = 0; i < leds.numPixels(); ++i) {
    leds.setPixel(i, 0xffffff);
  }
  for (int y = 0; y < LedsY; ++y) {
    for (int x = 0; x < LedsX; ++x) {
      leds.setPixel(LedMap[y * LedsX + x], 0x000000);
    }
  }
  leds.show();
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
 /* for (int i = 0; i < leds.numPixels(); ++i) {
    leds.setPixel(i, rand());
  }
  leds.show();*/
}
