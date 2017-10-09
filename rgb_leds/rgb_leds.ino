#include <FastLED.h>
#define NUM_LEDS 12
#define LED_PIN 6

CRGB leds[NUM_LEDS];

void setup()  {
  Serial.begin (115200);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(255, 255, 0);
    FastLED.show();
  }
}

void loop()  {
  /*for (int i = 0; i < NUM_LEDS; i++) {
    for (int x = 0; x < 3; x++) {
      if (x == 0) {
        leds[i].setRGB(0, 255, 0);
      } else if (x == 1) {
        leds[i].setRGB(255, 0, 0);
      } else {
        leds[i].setRGB(0, 0, 255);
      }

      FastLED.show();
      delay(100);
    }
  }*/
}
