#define BLYNK_TEMPLATE_ID "TMPL6OXV1bmFt"
#define BLYNK_TEMPLATE_NAME "Support"
#define BLYNK_AUTH_TOKEN "7sISC1fJBO2OhzgOrlCUbspnne6I3LWr"
#define BLYNK_FIRMWARE_VERSION "T2.PRE.070923"

#define BLYNK_PRINT Serial
#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_name = "http://sgp1.blynk.cloud/external/api/";
String Main = "ESzia3fpA-29cs8gt85pGnrPq_rICcqf";

const char* ssid = "Nha May Nuoc So 2";
const char* password = "02723841572";

//----------------------------------
float timerun, value = 0, value_s = 0, Result1, Result1_s;
int i = 0, j = 0;
bool keyPRE2 = true, keyPRE4 = true;

WidgetTerminal terminal(V0);
BlynkTimer timer;
BLYNK_CONNECTED() {

}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void updata() {
  String server_path = server_name + "batch/update?token=" + Main
                       + "&V10=" + Result1
                       + "&V38=" + 1;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  http.end();
}

void readPressure() {
  float sensorValue = analogRead(A0);
  float Result, Result_s;
  Result = (((sensorValue - 197) * 10) / (914.6 - 197)); //
  if (Result > 0)
  {
    value += Result;
    Result1 = value / 32.0;
    value -= Result1;
  }
  /*
  Result_s = sensorValue;
  if (Result_s > 0)
  {
    value_s += Result_s;
    Result1_s = value_s / 32.0;
    value_s -= Result1_s;
  }
  Serial.println(Result1_s);
  */
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(5000);

  timer.setInterval(1603L, updata);
  timer.setInterval(175L, readPressure);
}
void loop() {
  Blynk.run();
  timer.run();
}