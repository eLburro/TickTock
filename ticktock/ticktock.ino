#include <FastLED.h>
#define NUM_LEDS 6
#define LED_PIN 6
#define ROTARY_PIN_A 2
#define ROTARY_PIN_B 3

const int INTERVAL = 3;

volatile boolean fired = false;
volatile long rotaryCount = 0;
volatile int tmpIntervalPos = 0;
volatile int tmpIntervalNeg = 0;
long oldRotaryCount = 0;
int ledLightUp = 0;

CRGB leds[NUM_LEDS];

void countUp() {
  tmpIntervalNeg = 0;
  tmpIntervalPos++;

  if (tmpIntervalPos == INTERVAL) {
    rotaryCount++;
    tmpIntervalPos = 0;
  }
}

void countDown() {
  tmpIntervalPos = 0;
  tmpIntervalNeg++;

  if (tmpIntervalNeg == INTERVAL) {
    rotaryCount--;
    tmpIntervalNeg = 0;
  }
}

// Interrupt Service Routine by Nick Gammon
void isr() {
  static boolean ready;
  static byte pinA, pinB;

  // wait for main program to process it
  if (fired)
    return;

  byte newPinA = digitalRead(ROTARY_PIN_A);
  byte newPinB = digitalRead(ROTARY_PIN_B);

  // Forward is: LH/HH or HL/LL
  // Reverse is: HL/HH or LH/LL

  // so we only record a turn on both the same (HH or LL)
  if (newPinA == newPinB) {
    if (ready) {
      
      if (newPinA == HIGH) {  // must be HH now
        if (pinA == LOW) {
          countUp();
        } else {
          countDown();
        }
      } else { // must be LL now
        if (pinA == LOW) {
          countDown();
        } else {
          countUp();
        }
      }

      fired = true;
      ready = false;
    }  // end of being ready

    // end of completed click
  } else {
    ready = true;
  }

  pinA = newPinA;
  pinB = newPinB;
}

void reset() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(0, 0, 0);
    FastLED.show();
  }

  // activate pull-up resistors
  digitalWrite(ROTARY_PIN_A, HIGH);
  digitalWrite(ROTARY_PIN_B, HIGH);

  attachInterrupt(0, isr, CHANGE);   // ROTARY_PIN_A
  attachInterrupt(1, isr, CHANGE);   // ROTARY_PIN_B
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

    } else if (oldRotaryCount > rotaryCount) {
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
