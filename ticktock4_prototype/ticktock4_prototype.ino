#include <FastLED.h>
#define MAX_STEP 5
#define LED_PIN 6
#define ROTARY_PIN_A 2
#define ROTARY_PIN_B 3

unsigned long start, current, paused, goal;
int stepCount = 0;
const int INTERVAL = 3;
const int MINUTE = 10;

volatile boolean fired = false;
volatile long rotaryCount = 0;
volatile long oldRotaryCount = 0;
int tmpIntervalPos = 0;
int tmpIntervalNeg = 0;


CRGB leds[MAX_STEP];

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
  for (int i = 0; i < MAX_STEP; i++) {
    leds[i].setRGB(0, 0, 0);
    FastLED.show();
  }

  // activate pull-up resistors
  digitalWrite(ROTARY_PIN_A, HIGH);
  digitalWrite(ROTARY_PIN_B, HIGH);

  attachInterrupt(0, isr, CHANGE);   // ROTARY_PIN_A
  attachInterrupt(1, isr, CHANGE);   // ROTARY_PIN_B

  start=0;
  paused=0;
  goal=0;
  stepCount = 0;
  
  
}

void setup()  {
  Serial.begin (115200);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, MAX_STEP);
  reset();
}


void pause(){
  
}



void loop()  {

  if (fired)
  {
    //Serial.print ("Count = ");
    //Serial.println (rotaryCount);
    fired = false;

    if (oldRotaryCount < rotaryCount) {
      // clockwise
      Serial.println ("clockwise");
      start = millis() / 1000;
      Serial.print ("Step count before enters the if: ");
        Serial.println (stepCount);
      if (stepCount > 0) {
        Serial.print ("Step count after entering the if: ");
        Serial.println (stepCount);
        //turning off the led
     
        
        stepCount--;
        Serial.print ("Step count is decreased = ");
        Serial.println (stepCount);
        goal = stepCount * MINUTE;
        Serial.print ("Goal is = ");
        Serial.println (goal);

         Serial.println ("turning of the led");
        leds[stepCount].setRGB(0, 0, 0);
        FastLED.show();

      }

    } else if (oldRotaryCount > rotaryCount) {
      // counter-clockwise
      Serial.println ("counterclockwise");
      start = millis() / 1000;

        if (stepCount < MAX_STEP) {
          //turning on the led
          leds[stepCount].setRGB(0, 255, 0);
          FastLED.show();
        
          stepCount++;
          Serial.print ("Step count is = ");
          Serial.println (stepCount);
          goal = stepCount * MINUTE;
          Serial.print ("Goal is = ");
          Serial.println (goal);

        }
      }

      oldRotaryCount = rotaryCount;
    }

    //getting the current clock time
    current = millis() / 1000;

  
    //checks if the operation time is bigger than a minute
    
    if (((current - start) > MINUTE) && (stepCount!=0)) {
      
      start = start + MINUTE;
      Serial.print ("Time passed the step = ");
      Serial.println (stepCount);
      
      if (stepCount > 0) {
        
        stepCount--;
        Serial.print ("Step count is = ");
        Serial.println (stepCount);
      }
      
      leds[stepCount].setRGB(0, 0, 0);
      FastLED.show();

      
    }
    

  


  }
