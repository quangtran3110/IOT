#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D2
#define DHTPIN D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

char auth[] = "_8kF3uaNzdzWTTZquA63G-KQUz1ADvwA";
const char* ssid = "Wifi";
const char* password = "Password";
//const char* ssid = "tram bom so 4";
//const char* password = "0943950555";

BlynkTimer timer;
BLYNK_CONNECTED() {
}

void printTemperature(DeviceAddress deviceAddress) {
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC));  // Converts tempC to Fahrenheit
  Blynk.virtualWrite(V1, tempC);
}

void read() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  sensors.requestTemperatures();
  printTemperature(insideThermometer);
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V2, h);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(auth, "Blynkkwaco.ddns.net", 8080);
  delay(5000);
  //-------------------------------
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  sensors.setResolution(insideThermometer, 9);
  dht.begin();
  timer.setInterval(3031, read);
}

void loop() {
  timer.run();
  Blynk.run();
}
