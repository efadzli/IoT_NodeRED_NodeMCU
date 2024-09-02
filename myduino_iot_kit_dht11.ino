/*  DHT LIBRARY
 *  To use SimpleDHT.h, you need to install the library:
 *  Go to Sketch > Include Library > Manage Libraries...
 *  Search for 'SimpleDHT' and Install SimpleDHT by Winlin
 *  
 *  PIN CONNECTION
 *  NodeMCU 3v3 -> DHT11 5v
 *  NodeMCU GND -> DHT11 GND
 *  NodeMCU D3 -> DHT11 D
*/
#include <SimpleDHT.h>

int pinDHT11 = D3;
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(115200);
}

void loop() {
  // start working...
  Serial.println("=================================");
  Serial.println("Sensor reading:");
  
  // initialize variables
  byte temperature = 0;
  byte humidity = 0;

  // read from sensor
  dht11.read(&temperature, &humidity, NULL);   

  // print readings
  Serial.print("Data: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  
  // DHT11 sampling rate is 1HZ.
  delay(1500);
}
