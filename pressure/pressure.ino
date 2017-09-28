const int sensorPin = A0;
int value; //save analog value

void setup(){ 
  Serial.begin(9600);       

}

void loop(){
  value = analogRead(sensorPin);       //Read and save analog value from potentiometer
  Serial.println(value);               //Print value
}


