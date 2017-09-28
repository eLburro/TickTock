#include <FastLED.h>
#define MAX_STEP 5
#define LED_PIN 6
#define ROTARY_PIN_A 2
#define ROTARY_PIN_B 3

const int pressureSensorPin = A0;
int pressureValue; //save analog value

unsigned long startTime, currentTime, pausedTime,  breakTime, goal;
int stepCount = 0;
const int INTERVAL = 3;
const int MINUTE = 10;

volatile boolean fired = false;
volatile long rotaryCount = 0;
volatile long oldRotaryCount = 0;
int tmpIntervalPos = 0;
int tmpIntervalNeg = 0;
boolean paused = false;


CRGB leds[MAX_STEP];


void setup()  {
  Serial.begin (115200);
  
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, MAX_STEP);
  reset();
}



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

  startTime = 0;
  pausedTime = 0;
  goal = 0;
  stepCount = 0;
  paused = false;
  breakTime= 0;

}


void pause() {

  paused = true;
  pausedTime = millis() / 1000;
  Serial.print("The paused time starts: ");
  Serial.println(pausedTime);
 
}

void activate(){
  
      paused=false;
      currentTime = millis() / 1000;
      
      Serial.print("The paused time: ");
      Serial.println(currentTime - pausedTime);

      //if more than one pause is given
      breakTime = breakTime + (currentTime - pausedTime);
      // I couldnt just substract from the start time , cause when it goes below zero, it gives random
}



void loop()  {

  pressureValue = analogRead(pressureSensorPin);       //Read and save analog value from potentiometer


  if ((pressureValue > 500) && startTime != 0) {
    //check if it is already paused
    if (!paused) {
      pause();
    }

  } else {
    //
      
    // the countdown is activated after the pause
    if (paused && startTime != 0) {
      
        activate();
    }

    if (fired) {


      fired = false;

      if (oldRotaryCount < rotaryCount) {
        // clockwise
        Serial.println ("clockwise");
        startTime = millis() / 1000;
        Serial.print("The start time time: ");
        Serial.println(startTime);

        if (stepCount > 0) {


          stepCount--;
          Serial.print ("Step count is = ");
          Serial.println (stepCount);


          goal = stepCount * MINUTE;
          Serial.print ("Goal is = ");
          Serial.println (goal);


          leds[stepCount].setRGB(0, 0, 0);
          FastLED.show();

        }

      } else if (oldRotaryCount > rotaryCount) {
        // counter-clockwise
        Serial.println ("counterclockwise");
        startTime = millis() / 1000;
        Serial.print("The start time time: ");
        Serial.println(startTime);

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
    currentTime = millis() / 1000;

    //checks if the operation time is bigger than a minute

    if ((((currentTime - startTime) - breakTime) > MINUTE) && (stepCount != 0)) {

      startTime = startTime + MINUTE;
      Serial.print (" passed the step = ");
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



}
