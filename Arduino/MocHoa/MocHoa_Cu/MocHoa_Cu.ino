#define BLYNK_TEMPLATE_ID "TMPL7Z9cnaGi"
#define BLYNK_TEMPLATE_NAME "Support1"
#define BLYNK_AUTH_TOKEN "d54fFy8o6_fj09EOfaIFuZY_KwwmRNEw"
#define BLYNK_FIRMWARE_VERSION "MH-cu.23.10.1"
const char* ssid = "Cap Nuoc Moc Hoa ";
const char* password = "capnuoc@1992";
//const char* ssid = "Wifi";
//const char* password = "Password";
#define BLYNK_PRINT Serial
#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
//---------------
#include "EmonLib.h"
EnergyMonitor emon0;
//---------------
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_name = "http://sgp1.blynk.cloud/external/api/";
String Main = "tKNZ99XnCSeCsoDEva3kx-O0YWw83nMn";
//---------------
long m = 60000;
double Irms0 = 0;

BlynkTimer timer;
BLYNK_CONNECTED() {
}
//-------------------------------------------------------------------
void updata() {
  String server_path = server_name + "batch/update?token=" + Main
                       + "&V20=" + Irms0;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
  }
  http.end();
}
//-------------------------------------------------------------------
void readPower()  // Cấp 1 - 1 - I0
{
  float rms0 = emon0.calcIrms(1480);
  if (rms0 < 1) {
    rms0 = 0;
  }
  Irms0 = rms0;
  //Blynk.virtualWrite(V20, Irms0);
}
//-------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(10000);
  //---------------------------------------------------------------------------------

  emon0.current(A0, 28.5);

  timer.setInterval(1583L, []() {
    readPower();
    updata();
  });
}
void loop() {
  Blynk.run();
  timer.run();
}
