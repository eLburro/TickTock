unsigned long currentTime;
unsigned long loopTime;
const int pin_A = 12;
const int pin_B = 11;
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev = 0;

void setup()  {
  pinMode(pin_A, INPUT);
  pinMode(pin_B, INPUT);
  currentTime = millis();
  loopTime = currentTime;

  Serial.begin(9600);

  Serial.print("WAT");
}

void loop()  {
  currentTime = millis();
Serial.print("WAT");
  if (currentTime >= (loopTime + 5)) {
    // 5ms since last check of encoder = 200Hz
    encoder_A = digitalRead(pin_A);
    encoder_B = digitalRead(pin_B);

    if ((!encoder_A) && (encoder_A_prev)) {
      // A has gone from high to low
      if (encoder_B) {
        // B is high so clockwise
        Serial.println("clockwise");
      }
      else {
        // B is low so counter-clockwise
        Serial.println("counter-clockwise");
      }

    }
    encoder_A_prev = encoder_A;     // Store value of A for next time

    loopTime = currentTime;
  }

}
