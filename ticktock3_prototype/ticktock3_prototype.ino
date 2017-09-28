#include <FastLED.h>
#define NUM_LEDS 6
#define LED_PIN 6
#define ROTARY_PIN_A 2
#define ROTARY_PIN_B 3

unsigned long start, current, paused, goal;
int maxStep = 5;
int stepCount = 0;
const int INTERVAL = 3;
const long MINUTE = 20;

volatile boolean fired = false;
volatile long rotaryCount = 0;
volatile long oldRotaryCount = 0;
int tmpIntervalPos = 0;
int tmpIntervalNeg = 0;
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
      start = millis() / 1000;
      if (ledLightUp >= 0) {
        leds[ledLightUp].setRGB(0, 0, 0);
        FastLED.show();

        if (ledLightUp > 0) {
          ledLightUp--;
        }
      }

      if (stepCount > 0) {

        stepCount--;
        goal = stepCount * MINUTE;
        Serial.print ("Goal is = ");
        Serial.println (goal);

      }

    } else if (oldRotaryCount > rotaryCount) {
      // counter-clockwise
      start = millis() / 1000;
      if (ledLightUp < NUM_LEDS) {
        leds[ledLightUp].setRGB(0, 255, 0);
        FastLED.show();

        if (ledLightUp < (NUM_LEDS - 1)) {
          ledLightUp++;
        }
      }


      if (stepCount < maxStep) {
        stepCount++;
        goal = stepCount * MINUTE;
        Serial.print ("Goal is = ");
        Serial.println (goal);

      }
    }

    oldRotaryCount = rotaryCount;
  }

  //for (int i = 1; i <= goal / MINUTE; i++) {
  current = millis() / 1000;

  if ((current - start) > MINUTE) {
    start = start + MINUTE;
    Serial.print ("Time passed the step = ");
    Serial.println (ledLightUp);

    leds[ledLightUp].setRGB(0, 0, 0);
    FastLED.show();

    if (ledLightUp > 0) {
      ledLightUp--;
    }
  }

  //Serial.println ("");


}
