/******* VARIABLES & DEFINITIONS ********/
// libraries
#include <FastLED.h>

// define ports
#define DIG_PIN_ROTARY_A 2
#define DIG_PIN_ROTARY_B 3
#define DIG_PIN_VIBRATOR 4
#define DIG_PIN_LED 6
#define ANA_PIN_PRESSURE A0
#define ANA_PIN_AXIS_X A1
#define ANA_PIN_AXIS_Y A2
#define ANA_PIN_AXIS_Z A3

// constants
const int MAX_STEPS = 12;
const int ROTARY_INTERVAL = 3;
const int SECONDS = 10;
const int SHAKE_INTENSITY = 500;

// shared variables between ISR and other functions
volatile boolean rotaryFired = false;
volatile long rotaryPosition = 0;
volatile long oldRotaryPosition = 0;

// variables
int tmpIntervalPos = 0;
int tmpIntervalNeg = 0;

int pressureValue = 0;

long start;
long current;
long paused;
long goal;
int stepCount = 0;

CRGB leds[MAX_STEPS];
/******* END VARIABLES & DEFINITIONS ********/


/******* ISR FUNCTIONS ********/
// helper function to detect how much has to rotary encoded to be turned
void countUp() {
  tmpIntervalNeg = 0;
  tmpIntervalPos++;

  if (tmpIntervalPos == ROTARY_INTERVAL) {
    rotaryPosition++;
    tmpIntervalPos = 0;
  }
}

void countDown() {
  tmpIntervalPos = 0;
  tmpIntervalNeg++;

  if (tmpIntervalNeg == ROTARY_INTERVAL) {
    rotaryPosition--;
    tmpIntervalNeg = 0;
  }
}

// Interrupt Service Routine by Nick Gammon
void isr() {
  static boolean ready;
  static byte pinA, pinB;

  // wait for main program to process it
  if (rotaryFired)
    return;

  byte newPinA = digitalRead(DIG_PIN_ROTARY_A);
  byte newPinB = digitalRead(DIG_PIN_ROTARY_B);

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

      rotaryFired = true;
      ready = false;
    }  // end of being ready

    // end of completed click
  } else {
    ready = true;
  }

  pinA = newPinA;
  pinB = newPinB;
}
/******* END ISR FUNCTIONS ********/


/******* FUNCTIONS ********/
// reset the leds and variables
void reset() {
  // turn leds black
  for (int i = 0; i < MAX_STEPS; i++) {
    leds[i].setRGB(0, 0, 0);
    FastLED.show();
  }

  // activate pull-up resistors
  digitalWrite(DIG_PIN_ROTARY_A, HIGH);
  digitalWrite(DIG_PIN_ROTARY_B, HIGH);

  // attach the isr to the digital rotary pins
  attachInterrupt(0, isr, CHANGE);   // DIG_PIN_ROTARY_A
  attachInterrupt(1, isr, CHANGE);   // DIG_PIN_ROTARY_B

  start = 0;
  paused = 0;
  goal = 0;
  stepCount = 0;
}

// check if a wifi signal appears to stop the application
boolean isEmergencyStop() {
  // TODO
  return false;
}

// same functionality as Arduino's standard map function, except using floats
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// read the shake intensity and return true if any axis is shaked harder than SHAKE_INTENSITY
boolean isShakingResetActivated() {
  // get raw accelerometer data for each axis
  int rawX = analogRead(ANA_PIN_AXIS_X);
  int rawY = analogRead(ANA_PIN_AXIS_Y);
  int rawZ = analogRead(ANA_PIN_AXIS_Z);

  // ccale accelerometer ADC readings into common units
  float scaledX;
  float scaledY;
  float scaledZ;
  int scale = 200;

  scaledX = mapf(rawX, 0, 675, -scale, scale); // 3.3/5 * 1023 =~ 675
  scaledY = mapf(rawY, 0, 675, -scale, scale);
  scaledZ = mapf(rawZ, 0, 675, -scale, scale);

  return (scaledX > SHAKE_INTENSITY || scaledY > SHAKE_INTENSITY || scaledZ > SHAKE_INTENSITY);
}

// check the pressure sensor if the robot has been flipped over
// if yes then the application pauses
boolean isTimerPaused() {
  // read and save analog value from potentiometer 0-1023
  pressureValue = analogRead(ANA_PIN_PRESSURE);

  return (pressureValue > 750);
}

// utility to change the color of a specific LED
void changeLEDColor(int ledNum, int g, int r, int b) {
  leds[ledNum].setRGB(g, r, b);
  FastLED.show();
}

// makes the vibrator vibrate for the requested time
void vibrate(long milliSeconds) {
  Serial.print("Vibrate for ");
  Serial.print(milliSeconds);
  Serial.print(" milliseconds");
  Serial.println("");

  for (int i = 0; i < milliSeconds; i + 200) {
    analogWrite(DIG_PIN_VIBRATOR, 255);
    delay(100);
    analogWrite(DIG_PIN_VIBRATOR, 0);
    delay(100);
  }
}

// update the leds and timers based on the manual rotation
void updateRotaryPosition() {
  rotaryFired = false;
  start = millis() / 1000;

  if (oldRotaryPosition < rotaryPosition) {
    // clockwise rotation
    Serial.println ("clockwise");
    Serial.print ("Step count: ");
    Serial.println (stepCount);

    if (stepCount > 0) {
      stepCount--;
      goal = stepCount * SECONDS;

      //turning off the led
      changeLEDColor(stepCount, 0, 0, 0);

      Serial.print ("Step count is decreased = ");
      Serial.println (stepCount);
      Serial.print ("Goal is = ");
      Serial.println (goal);
    }

  } else if (oldRotaryPosition > rotaryPosition) {
    // counter-clockwise rotation
    Serial.println ("counterclockwise");

    if (stepCount < MAX_STEPS) {
      //turning on the led
      changeLEDColor(stepCount, 0, 255, 0);

      // TODO check if this is really after turning on led
      stepCount++;
      goal = stepCount * SECONDS;

      Serial.print ("Step count is = ");
      Serial.println (stepCount);
      Serial.print ("Goal is = ");
      Serial.println (goal);
    }
  }

  oldRotaryPosition = rotaryPosition;
}

// check the timers and update the leds automatically
void updateLeds() {
  //getting the current clock time
  current = millis() / 1000;

  //checks if the operation time is bigger than the SECONDS
  if (((current - start) > SECONDS) && (stepCount != 0)) {
    start += SECONDS;

    Serial.print ("Time passed the step = ");
    Serial.println (stepCount);

    if (stepCount > 0) {
      stepCount--;

      Serial.print ("Step count is = ");
      Serial.println (stepCount);
    }

    // turn off led
    changeLEDColor(stepCount, 0, 0, 0);
  }
}
/******* END FUNCTIONS ********/


/******* SETUP & LOOP ********/
void setup()  {
  Serial.begin (115200);
  FastLED.addLeds<NEOPIXEL, DIG_PIN_LED>(leds, MAX_STEPS);
  pinMode(DIG_PIN_VIBRATOR, OUTPUT);

  reset();
}

void loop()  {
  // stop application when no emergency broadcast signal appears
  if (isEmergencyStop() || isShakingResetActivated()) {
    reset();

  } else {

    // check if the rotary encoder was moved
    if (rotaryFired) {
      updateRotaryPosition();
    }

    // check if robot has flipped and paused
    if (!isTimerPaused) {
      updateLeds();
    }
  }
}
/******* END SETUP & LOOP ********/
