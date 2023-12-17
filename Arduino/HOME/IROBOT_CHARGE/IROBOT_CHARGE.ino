#include <BlynkSimpleEsp8266.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>

#define I2C_ADDRESS 0x40

char auth[] = "L9MuvVtLD5XFsHjds9dE5L8GFmOCoSUR";
const char* ssid = "Wifi";
const char* password = "Password";

bool key = true;
BlynkTimer timer;
BLYNK_CONNECTED() {
}

BLYNK_WRITE(V5) {
  if (param.asInt() == 0) {
    if (key) {
      key = false;
      digitalWrite(D1, HIGH);
      timer.setTimeout(30000, []() {
        key = true;
        digitalWrite(D1, LOW);
      });
    }
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(auth, "Blynkkwaco.ddns.net", 8080);
  delay(5000);

  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
}

void loop() {
  timer.run();
  Blynk.run();
}