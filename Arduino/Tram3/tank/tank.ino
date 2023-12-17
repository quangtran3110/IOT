#define BLYNK_TEMPLATE_ID "TMPL6OXV1bmFt"
#define BLYNK_TEMPLATE_NAME "Support"
#define BLYNK_AUTH_TOKEN "7sISC1fJBO2OhzgOrlCUbspnne6I3LWr"
#define BLYNK_FIRMWARE_VERSION "T3.TANK"

#define BLYNK_PRINT Serial
#define APP_DEBUG
//----------------------
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
const char* ssid = "Phong Tai Vu";
const char* password = "0974040699";
//----------------------
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_name = "http://sgp1.blynk.cloud/external/api/";
String Main = "lvmyPh1nGcSjs3n1CTJrX4DyuWLlS0i0";
//----------------------
#define filterSamples 121
//----------------------------------
const int S0 = 14;
const int S1 = 12;
const int S2 = 13;
const int S3 = 15;

const int dotzerotank = 205;
const int dotmaxtank = 940;
const int dotzeropre = 202;
const int dotmaxpre = 1018;
//----------------------------------
float pi = 3.14;
float bankinh2 = 235 * 235;
float dosau = 220;

float value14 = 0, Result1;
float volume, volume1, percent, percent1, dungtich, smoothDistance;

int sensSmoothArray1[filterSamples];
int digitalSmooth(int rawIn, int* sensSmoothArray) {
  int j, k, temp, top, bottom;
  long total;
  static int i;
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;  // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;   // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j = 0; j < filterSamples; j++) {  // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;            // flag to know when we're done sorting
  while (done != 1) {  // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {  // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted[j + 1] = sorted[j];
        sorted[j] = temp;
        done = 0;
      }
    }
  }
  bottom = max(((filterSamples * 20) / 100), 1);
  top = min((((filterSamples * 80) / 100) + 1), (filterSamples - 1));  // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for (j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
  }
  return total / k;  // divide by number of samples
}

long distance, distance1;

WidgetTerminal terminal(V0);
BlynkTimer timer;
BLYNK_CONNECTED() {
}
//------------------------------------------------
void updata() {
  String server_path = server_name + "batch/update?token=" + Main
                       + "&V5=" + smoothDistance
                       + "&V6=" + volume1
                       + "&V4=" + Result1
                       + "&V21=" + 1;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
  }
  http.end();
}
//------------------------------------------------
void MeasureCmForSmoothing() {
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float sensorValue = analogRead(A0);
  distance1 = (((sensorValue - dotzerotank) * 500) / (dotmaxtank - dotzerotank));  //915,74 (R=147.7)
  if (distance1 > 0) {
    smoothDistance = digitalSmooth(distance1, sensSmoothArray1);
    volume1 = (pi * smoothDistance * bankinh2) / 1000000;
  }
  //Serial.print("sensorValue: ");
  //Serial.println(sensorValue);
  //Serial.println(distance1);
}
//----------------------------------------------------
void readPressure() {  //pin C1
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float sensorValue = analogRead(A0);
  float Result;
  Result = (((sensorValue - dotzeropre) * 10) / (dotmaxpre - dotzeropre));
  //Serial.print("sensorValue: ");
  //Serial.println(Result1);
  //Serial.println(Result1);
  if (Result > 0) {
    value14 += Result;
    Result1 = value14 / 16.0;
    value14 -= Result1;
  }
}

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(5000);

  timer.setInterval(1503L, updata);
  timer.setInterval(215L, []() {
    readPressure();
    MeasureCmForSmoothing();
  });
}
void loop() {
  Blynk.run();
  timer.run();
}