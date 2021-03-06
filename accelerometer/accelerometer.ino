int scale = 200;
boolean micro_is_5V = true;
//int a = 0;
void setup()  {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
}

void loop()
{
  // Get raw accelerometer data for each axis
  int rawX = analogRead(A0);
  //int rawY = analogRead(A1);
  //int rawZ = analogRead(A2);

  // Scale accelerometer ADC readings into common units
  // Scale map depends on if using a 5V or 3.3V microcontroller
  float scaledX, scaledY, scaledZ; // Scaled values for each axis
  if (micro_is_5V) // Microcontroller runs off 5V
  {
    scaledX = mapf(rawX, 0, 675, -scale, scale); // 3.3/5 * 1023 =~ 675
    //scaledY = mapf(rawY, 0, 675, -scale, scale);
    //scaledZ = mapf(rawZ, 0, 675, -scale, scale);
  }
  else // Microcontroller runs off 3.3V
  {
    scaledX = mapf(rawX, 0, 1023, -scale, scale);
    //scaledY = mapf(rawY, 0, 1023, -scale, scale);
    //scaledZ = mapf(rawZ, 0, 1023, -scale, scale);
  }

  // Print out scaled X,Y,Z accelerometer readings
  Serial.print("X: "); Serial.print(scaledX); Serial.println(" g");
  //Serial.print("Y: "); Serial.print(scaledY); Serial.println(" g");
  //Serial.print("Z: "); Serial.print(scaledZ); Serial.println(" g");
  Serial.println();

  delay(2000); // Minimum delay of 2 milliseconds between sensor reads (500 Hz)
}

// Same functionality as Arduino's standard map function, except using floats
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
