#include <WiFi.h>

int temp_pin= 24;
int led1_pin= 2;

void setup(void)
{ 
  Serial.begin(9600);
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);
}

void loop() {  
  int analogValue = analogRead(temp_pin); // get reading from LM35 temperature sensor
  float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
  float celsius = millivolts/10;

  Serial.print("Temperature = ");
  Serial.print(celsius);
  Serial.println(" °C");

  if(digitalRead(led1_pin)==0)
    digitalWrite(led1_pin, HIGH);
  else
    digitalWrite(led1_pin, LOW);

  delay(2000);
}
