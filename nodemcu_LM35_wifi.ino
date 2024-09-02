#include "ESP8266WiFi.h"
#include <MQTT.h>

#define WIFI_SSID             "xxxxx"
#define WIFI_PASSWORD         "yyyyy"
#define MQTT_HOST             "broker.hivemq.com"
#define MQTT_PREFIX_TOPIC     "RoboPRO/iot"
#define MQTT_PUBLISH_TOPIC    "/weather"
#define MQTT_SUBSCRIBE_TOPIC1  "/led1"
#define MQTT_SUBSCRIBE_TOPIC2  "/led2"

WiFiClient net;
MQTTClient mqtt(1024);
unsigned long lastMillis = 0;
int temp_pin= A0;
int led1_pin= D5;

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
  Serial.println("Subscribe to: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC2));
  
  mqtt.subscribe(String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1));
  mqtt.subscribe(String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC2));

}


void setup(void)
{ 
  Serial.begin(9600);
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);

  connectToWiFi();
  connectToMqttBroker();

  Serial.println();

}
void loop() {
  mqtt.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!mqtt.connected()) {
    connectToMqttBroker();
  }

  int analogValue = analogRead(temp_pin);
  float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
  float celsius = millivolts/10;

  int ledValue = digitalRead(led1_pin);

  Serial.print("Temperature = ");
  Serial.print(celsius);
  Serial.println(" °C");

  Serial.print("LED1 = ");
  Serial.println(ledValue);

  if (millis() - lastMillis > 10000) { // publish every 10 seconds
    lastMillis = millis();

    String dataInJson = "{";
    dataInJson += "\"temperature\":" + String(celsius) + ",";
    dataInJson += "\"led1\":" + String(ledValue);
    dataInJson += "}";

    Serial.println("Data to Publish: " + dataInJson);
    Serial.println("Length of Data: " + String(dataInJson.length()));
    Serial.println("Publish to: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC));
    
    mqtt.publish(String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC), dataInJson);
  }

  Serial.println();

  delay(5000);
}
