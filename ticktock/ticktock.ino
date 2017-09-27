#include <FastLED.h>
#define NUM_LEDS 6
#define LED_PIN 6
#define ROTARY_PIN_A 2
#define ROTARY_PIN_B 3

volatile boolean fired = false;
volatile long rotaryCount = 0;
volatile long oldRotaryCount = 0;
int ledLightUp = 0;

CRGB leds[NUM_LEDS];

// Interrupt Service Routine by Nick Gammon
void isr () {

  static boolean ready;
  static unsigned long lastFiredTime;
  static byte pinA, pinB;

  // wait for main program to process it
  if (fired)
    return;

  byte newPinA = digitalRead (ROTARY_PIN_A);
  byte newPinB = digitalRead (ROTARY_PIN_B);

  // Forward is: LH/HH or HL/LL
  // Reverse is: HL/HH or LH/LL

  // so we only record a turn on both the same (HH or LL)

  if (newPinA == newPinB)
  {
    if (ready)
    {
      long increment = 1;

      // if they turn the encoder faster, make the count go up more
      // (use for humans, not for measuring ticks on a machine)
      unsigned long now = millis ();
      unsigned long interval = now - lastFiredTime;
      lastFiredTime = now;

      if (interval < 100)
        increment = 5;
      else if (interval < 200)
        increment = 3;
      else if (interval < 500)
        increment = 2;

      if (newPinA == HIGH)  // must be HH now
      {
        if (pinA == LOW)
          rotaryCount += increment;
        else
          rotaryCount -= increment;
      }
      else
      { // must be LL now
        if (pinA == LOW)
          rotaryCount -= increment;
        else
          rotaryCount += increment;
      }
      fired = true;
      ready = false;
    }  // end of being ready
  }  // end of completed click
  else
    ready = true;

  pinA = newPinA;
  pinB = newPinB;
}

void reset() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(0, 0, 0);
    FastLED.show();
  }

  // activate pull-up resistors
  digitalWrite (2, HIGH);
  digitalWrite (3, HIGH);

  attachInterrupt (0, isr, CHANGE);   // pin 2
  attachInterrupt (1, isr, CHANGE);   // pin 3
}

void setup()  {
  Serial.begin (115200);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  reset();
}

void loop()  {

  if (fired)
  {
    Serial.print ("Count = ");
    Serial.println (rotaryCount);
    fired = false;

    if (oldRotaryCount < rotaryCount) {
      // clockwise

      if (ledLightUp >= 0) {
        leds[ledLightUp].setRGB(0, 0, 0);
        FastLED.show();

        if (ledLightUp > 0) {
          ledLightUp--;
        }
      }

    } else {
      // counter-clockwise

      if (ledLightUp < NUM_LEDS) {
        leds[ledLightUp].setRGB(0, 255, 0);
        FastLED.show();

        if (ledLightUp < (NUM_LEDS - 1)) {
          ledLightUp++;
        }
      }
    }

    oldRotaryCount = rotaryCount;
  }
}
