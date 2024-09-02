/*  DHT LIBRARY
 *  To use SimpleDHT.h, you need to install the library:
 *  Go to Sketch > Include Library > Manage Libraries...
 *  Search for 'SimpleDHT' and Install SimpleDHT by Winlin
 *  
 *  PIN CONNECTION
 *  NodeMCU 3v3 -> DHT11 5v
 *  NodeMCU GND -> DHT11 GND
 *  NodeMCU D3 -> DHT11 D
 *  
 *  WIFI LIBRARY
 *  To use SimpleDHT.h, you need to install the library:
 *  Go to Sketch > Include Library > Manage Libraries...
 *  Search for 'MQTT' and Install MQTT by Joel Gaehwiler
*/
#include <SimpleDHT.h>
#include "ESP8266WiFi.h"
#include <MQTT.h>

#define WIFI_SSID             "xxxxxxxx"
#define WIFI_PASSWORD         "yyyyyyyy"
#define MQTT_HOST             "broker.hivemq.com"
#define MQTT_PREFIX_TOPIC     "FSKM/iot"
#define MQTT_PUBLISH_TOPIC1    "/temp"
#define MQTT_PUBLISH_TOPIC2    "/humid"
#define MQTT_SUBSCRIBE_TOPIC1  "/led1"

WiFiClient net;
MQTTClient mqtt(1024);
unsigned long lastMillis = millis();
int pinDHT11 = D3;
int led1_pin = D4;
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(115200);
}

void loop() {
  mqtt.loop(); // execute mqtt protocol
  delay(10);  // fixes some issues with WiFi stability

  // check WiFi status, if not connected, then connect
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // check MQTT broker status, if not connected, then connect
  if (!mqtt.connected()) {
    connectToMqttBroker();
  }

  
  // start working...
  Serial.println("=================================");
  Serial.println("Sample Reading:");
  
  // initialize variables
  byte temperature = 0;
  byte humidity = 0;

  // read from sensor
  dht11.read(&temperature, &humidity, NULL);   

  // print sensor readings
  Serial.print("Data: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");

  if (millis() - lastMillis > 10000) { // publish every 10 seconds
    lastMillis = millis();

    // construct data into json format
    String dataInJson = "{";
    dataInJson += "\"temperature\":" + String(temperature) + ",";
    dataInJson += "\"humidity\":" + String(humidity);
    dataInJson += "}";

    Serial.println("Data to Publish: " + dataInJson);
    Serial.println("Length of Data: " + String(dataInJson.length()));
    Serial.println("Publish to: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC1));

    // publish data to MQTT Broker
    mqtt.publish(String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC1), dataInJson);
  }
  
  // DHT11 sampling rate is 1HZ.
  delay(1500);
}

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi '" + String(WIFI_SSID) + "' ...");

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming Status from topic " + topic + ": " + payload);
  Serial.println();
  
  if(topic.equals("RoboPRO/iot/led1") && payload.equals("1"))
    digitalWrite(led1_pin, HIGH);
  else if(topic.equals("RoboPRO/iot/led1") && payload.equals("0"))
    digitalWrite(led1_pin, LOW);
  
}

void connectToMqttBroker(){
  Serial.print("Connecting to '" + String(WIFI_SSID) + "' ...");
  
  mqtt.begin(MQTT_HOST, net);
  mqtt.onMessage(messageReceived);

  String uniqueString = String(WIFI_SSID) + "-" + String(random(1, 98)) + String(random(99, 999));
  char uniqueClientID[uniqueString.length() + 1];
  
  uniqueString.toCharArray(uniqueClientID, uniqueString.length() + 1);
  
  while (!mqtt.connect(uniqueClientID)) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");

  Serial.println("Subscribe to: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1));  
  mqtt.subscribe(String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1));

}
