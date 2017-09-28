/*
Adafruit Arduino - Lesson 13. DC Motor
*/
 
 
int motorPin = 3;
 
void setup() 
{ 
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Speed 0 to 255");
} 
 
 
void loop() 
{ 
  analogWrite(motorPin, 255);
  Serial.println("High");
  delay(1000);
  analogWrite(motorPin, 0);
  Serial.println("Low");
  delay(1000);
} 
