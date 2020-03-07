// TESTED ON ESP32 Dev Module

#include <WiFi.h>
#include "MQTToverSerial.h"

const char* ssid     = "SSID";
const char* password = "PASSWORD";

WiFiClient wifiClient;

MQTToverSerial* mqttClient;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("INFO$Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }

  Serial.println("INFO$WiFi connected");
  Serial.print("INFO$IP address: ");
  Serial.println(WiFi.localIP());

  mqttClient = new MQTToverSerial(wifiClient, Serial, "192.168.2.1", 1883, "", "", "ESP32-MQTToverSerial");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available())
    mqttClient->ReadSerial();
  mqttClient->Loop();
}
