#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLEpk-Eewt"
#define BLYNK_TEMPLATE_NAME "Trạm Cái Cát"
#define BLYNK_AUTH_TOKEN "OwqkFUOpl8p9-AP235SQquza0fmhwImP"

#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "EmonLib.h"

EnergyMonitor emon0, emon1;

const char* ssid = "Wifi";
const char* password = "Password";

const int S0 = D0;
const int S1 = D1;
const int S2 = D2;
const int S3 = D3;
const int RL1 = D4;
const int RL2 = D5;
const int RL3 = D6;
const int RL4 = D7;

bool statusRualoc1 = LOW, statusRualoc2 = LOW, status_NK1 = HIGH, status_NK2 = HIGH;
bool trip1 = false, trip2 = false, key = false;
int xSetAmpe = 0, xSetAmpe1 = 0;
int timer_I;
unsigned long int yIrms0 = 0, yIrms1 = 0;
float Irms0, Irms1;

WidgetTerminal keyterminal(V5);

BlynkTimer timer;
BLYNK_CONNECTED() {
}

int SetAmpemax = 12, SetAmpe1max = 12;
int SetAmpemin = 3, SetAmpe1min = 3;

String server_name = "http://sgp1.blynk.cloud/external/api/";
String Caicat_main = "OwqkFUOpl8p9-AP235SQquza0fmhwImP";

void send_data(String token, int virtual_pin, float value_to_send) {
  WiFiClient client;
  HTTPClient http;
  String server_path = server_name + "update?token=" + token + "&pin=v" + String(virtual_pin) + "&value=" + float(value_to_send);
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
  }
  http.end();
}
void connectionstatus() {
  if ((WiFi.status() != WL_CONNECTED)) {
    //Serial.println("Khong ket noi WIFI");
  }
  if ((WiFi.status() == WL_CONNECTED) && (!Blynk.connected())) {
    //Serial.println("Khong ket noi Internet");
    //WiFi.reconnect();
  }
  if (Blynk.connected()) {
    //Serial.println("Đã kết nối");
  }
}

void status_device() {
  Blynk.virtualWrite(V22, "Online");
}

void on_NK1() {
  digitalWrite(RL3, LOW);
}
void off_NK1() {
  digitalWrite(RL3, HIGH);
}
void on_NK2() {
  digitalWrite(RL4, LOW);
}
void off_NK2() {
  digitalWrite(RL4, HIGH);
}
void loc1() {
  if (statusRualoc1 == HIGH) {
    digitalWrite(RL1, LOW);
    timer.setTimeout((7 * 60 * 1000), []() {
      digitalWrite(RL1, HIGH);
    });
  }
}
void loc2() {
  if (statusRualoc2 == HIGH) {
    digitalWrite(RL2, LOW);
    timer.setTimeout((7 * 60 * 1000), []() {
      digitalWrite(RL2, HIGH);
    });
  }
}

void readcurrent()  // C0 - NK1
{
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float rms0 = emon0.calcIrms(1480);
  if (rms0 < 1) {
    Irms0 = 0;
    yIrms0 = 0;
  } else if (rms0 > 1) {
    Irms0 = rms0;
    yIrms0 = yIrms0 + 1;
    if ((yIrms0 > 2) && ((Irms0 > SetAmpemax) || (Irms0 < SetAmpemin))) {
      xSetAmpe = xSetAmpe + 1;
      if (xSetAmpe >= 3) {
        trip1 = true;
        off_NK1();
        Blynk.logEvent("error", String("Nén Khí 1 lỗi: ") + Irms0 + String(" A"));
        xSetAmpe = 0;
      }
    } else {
      xSetAmpe = 0;
    }
  }
  //Serial.println(Irms0);
  Blynk.virtualWrite(V41, Irms0);
}
void readcurrent1()  // C1 - NK 2
{
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float rms1 = emon1.calcIrms(1480);
  if (rms1 < 1) {
    Irms1 = 0;
    yIrms1 = 0;
  } else if (rms1 > 1) {
    Irms1 = rms1;
    yIrms1 = yIrms1 + 1;
    if ((yIrms1 > 2) && ((Irms1 > SetAmpe1max) || (Irms1 < SetAmpe1min))) {
      xSetAmpe1 = xSetAmpe1 + 1;
      if (xSetAmpe1 >= 3) {
        trip2 = true;
        off_NK2();
        Blynk.logEvent("error", String("Nén Khí 2 lỗi: ") + Irms1 + String(" A"));
        xSetAmpe1 = 0;
      }
    } else {
      xSetAmpe1 = 0;
    }
  }
  Blynk.virtualWrite(V42, Irms1);
}
BLYNK_WRITE(V5)  // data string
{
  String dataS = param.asStr();
  if (dataS == "reset") {
    keyterminal.clear();
    trip1 = false;
    trip2 = false;
    digitalWrite(RL3, LOW);
    digitalWrite(RL4, LOW);
    Blynk.virtualWrite(V5, "Đã reset Nén Khí!");
  } else if (dataS == "cc" || dataS == "CC") {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "NK kích hoạt trong 10s\n");
    key = true;
    timer.setTimeout(10000, []() {
      key = false;
      keyterminal.clear();
    });
  }
}
BLYNK_WRITE(V23) {
  if (param.asInt() == 0) {
    statusRualoc1 = LOW;
    statusRualoc2 = LOW;
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
  } else if (param.asInt() == 1) {
    statusRualoc1 = HIGH;
    statusRualoc2 = LOW;
    digitalWrite(RL1, LOW);
    digitalWrite(RL2, HIGH);
  } else if (param.asInt() == 2) {
    statusRualoc1 = LOW;
    statusRualoc2 = HIGH;
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, LOW);
  } else if (param.asInt() == 3) {
    statusRualoc1 = HIGH;
    statusRualoc2 = HIGH;
    digitalWrite(RL1, LOW);
    digitalWrite(RL2, LOW);
  }
}
BLYNK_WRITE(V43)  // NK1
{
  if (key && !trip1) {
    if (param.asInt() == HIGH) {
      status_NK1 = HIGH;
      on_NK1();
    } else {
      status_NK1 = LOW;
      off_NK1();
    }
  }
  Blynk.virtualWrite(V43, status_NK1);
}
BLYNK_WRITE(V44)  // NK2
{
  if (key && !trip2) {
    if (param.asInt() == HIGH) {
      status_NK2 = HIGH;
      on_NK2();
    } else {
      status_NK2 = LOW;
      off_NK2();
    }
  }
  Blynk.virtualWrite(V44, status_NK2);
}

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(RL1, OUTPUT);
  digitalWrite(RL1, HIGH);
  pinMode(RL2, OUTPUT);
  digitalWrite(RL2, HIGH);
  pinMode(RL3, OUTPUT);
  digitalWrite(RL3, LOW);
  pinMode(RL4, OUTPUT);
  digitalWrite(RL4, LOW);

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(7000);

  emon0.current(A0, 59);
  emon1.current(A0, 111);



  timer.setInterval((10 * 60 * 1000), loc1);
  timer.setInterval((10 * 61 * 1000), loc2);
  timer.setInterval(53303, status_device);
  timer_I = timer.setInterval(1589, []() {
    readcurrent();
    readcurrent1();
  });
  /*
  timer.setInterval(900005L, []() {
    connectionstatus();
    timer.restartTimer(timer_I);
  });
  */
}

void loop() {
  Blynk.run();
  timer.run();
}