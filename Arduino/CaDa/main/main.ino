/*
V0 - Irms0
V1 - Irms1
V2 - Ap luc
V3 - Btn
V4 - terminal
V5 -
V6 -
V7 - volume
V8 -
V9 - distence
V10-
V11- Dung tich
V12- dosau

*/
/*
#define BLYNK_TEMPLATE_ID "TMPLrdcYlz_1"
#define BLYNK_TEMPLATE_NAME "Trạm Cả Đá"
#define BLYNK_AUTH_TOKEN "HRuqR5DchX_9Nlk--FAFkQiLEaDtr1VV"
*/
#define BLYNK_TEMPLATE_ID "TMPL6MowNW4lw"
#define BLYNK_TEMPLATE_NAME "TRẠM CẢ ĐÁ"
#define BLYNK_AUTH_TOKEN "QZeQANTaoPgXFP9IkGlk84oZ4MDIpnSz"

#define BLYNK_FIRMWARE_VERSION "240503"
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include "EmonLib.h"

#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/IOT/main/Arduino/CaDa/main/build/esp8266.esp8266.nodemcuv2/main.ino.bin"
String server_name = "http://sgp1.blynk.cloud/external/api/";

#define filterSamples 121

const char* ssid = "Yen Nhi";
const char* password = "12345678";

//const char* ssid = "iPhone 13";
//const char* password = "12345687";

const int S0pin = 14;
const int S1pin = 12;
const int S2pin = 13;
const int S3pin = 15;
const int EN = 0;

float dai = 8.4;
float rong = 4;
float dosau = 2.32;

double Irms0, Irms1;

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

float volume, volume1, percent, percent1, dungtich, smoothDistance, value_tank, sensorValue1;
long distance, distance1;
float timerun, value = 0, Result1;
bool key = false;
char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
int watchdogcount = 0, RelayState = LOW, RelayState1 = LOW;
int btnState = HIGH, btnState1 = HIGH, AppState, AppState1, x = 1, u = 0;
int timer_pre, timer_tank;
byte reboot_num = 0;

BlynkTimer timer;
EnergyMonitor emon0, emon1;

WidgetTerminal terminal(V4);
BLYNK_CONNECTED() {
}

void upinfo() {
  // dungtich = dai * rong * dosau;
  // Blynk.virtualWrite(V11, dungtich);
  if (x == 0) {
    Blynk.virtualWrite(V3, 1);
  } else if (x == 1) {
    Blynk.virtualWrite(V3, 0);
  }
}
/*
void upinfo1()
{
  Blynk.virtualWrite(V12, dosau * 100);
  // Blynk.virtualWrite(V4, 2.1);
}
*/
//-------------------------------------------------------------
void Pressure() {
  // Ap luc
  // digitalWrite(EN, HIGH);
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  // digitalWrite(EN, LOW);
  float sensorValue = analogRead(A0);
  // Serial.println(sensorValue);
  //  digitalWrite(EN, HIGH);
  float Result;
  Result = (((sensorValue - 184) * 10) / (1038 - 184));
  if (Result > 0) {
    value += Result;
    Result1 = value / 16.0;
    value -= Result1;
  }
}
//----------------------------
BLYNK_WRITE(V3) {
  AppState = param.asInt();
  if (key) {
    if (AppState == 1) {
      digitalWrite(4, LOW);
      x = 0;
    } else if (AppState == 0) {
      digitalWrite(4, HIGH);
      x = 1;
    }
  }
  if (x == 0) {
    Blynk.virtualWrite(V3, 1);
  }
  if (x == 1) {
    Blynk.virtualWrite(V3, 0);
  }
}
//----------------------------
BLYNK_WRITE(V4) {
  String dataS = param.asStr();
  if ((dataS == "cd") || (dataS == "CD")) {
    terminal.clear();
    key = true;
    Blynk.virtualWrite(V4, "Đơn vị vận hành: Mộc Hóa'\nKích hoạt trong 15s\n");
    timer.setTimeout(15000L, []() {
      key = false;
      terminal.clear();
      Blynk.virtualWrite(V4, "Nhập mật mã!\n");
    });
  } else if (dataS == "active") {
    terminal.clear();
    key = true;
    Blynk.virtualWrite(V4, "Người vận hành: 'M.Quang'\nVui lòng không sử dụng phần mềm\ncho đến khi thông báo này mất.");
  } else if (dataS == "deactive") {
    terminal.clear();
    key = false;
    Blynk.virtualWrite(V4, "Nhập mật mã!\n");
  } else if (dataS == "rst") {
    terminal.clear();
    Blynk.virtualWrite(V4, "ESP RESTART.\n");
    delay(3000);
    ESP.restart();
  } else if (dataS == "update") {
    terminal.clear();
    Blynk.virtualWrite(V4, "UPDATE FIRMWARE...");
    update_fw();
  } else {
    key = false;
    Blynk.virtualWrite(V4, "Mật mã sai.\n");
    Blynk.virtualWrite(V4, "Hãy nhập lại!\n");
  }
}

//-------------------------------------------------------------
void readAnalog() {
  // Ampe 1
  digitalWrite(EN, HIGH);
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  Irms0 = emon0.calcIrms(740);
  // Serial.println(Irms0);
  digitalWrite(EN, HIGH);
  if (Irms0 < 3) {
    Irms0 = 0;
  }
  //Blynk.virtualWrite(V0, Irms0);
  // Ampe 2
  digitalWrite(EN, HIGH);
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  Irms1 = emon1.calcIrms(740);
  //Serial.println(Irms1);
  digitalWrite(EN, HIGH);
  if (Irms1 < 3) {
    Irms1 = 0;
  }
  //Blynk.virtualWrite(V1, Irms1);
  // Ap luc
}
//-------------------------------------------------------------
void MeasureCmForSmoothing() {
  digitalWrite(EN, HIGH);
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  float sensorValue = analogRead(A0);
  digitalWrite(EN, HIGH);
  if (sensorValue > 0) {
    value_tank += sensorValue;
    sensorValue1 = value_tank / 8.0;
    value_tank -= sensorValue1;
  }
  distance1 = (((sensorValue1 - 185) * 500) / (955 - 185));  // 915,74 (R=147.7)
  //Serial.println(distance1);
  if (distance1 > 0) {
    smoothDistance = digitalSmooth(distance1, sensSmoothArray1);
    volume1 = (dai * (smoothDistance / 100) * rong);
  }
}

void updata() {
  String server_path = server_name + "batch/update?token=" + BLYNK_AUTH_TOKEN
                       + "&V2=" + Result1
                       + "&V0=" + Irms0
                       + "&V1=" + Irms1
                       + "&V7=" + volume1
                       + "&V9=" + smoothDistance;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  http.end();
  Blynk.virtualWrite(V8, WiFi.RSSI());
}

void connectionstatus() {
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.println("Khong ket noi WIFI");
    reboot_num = reboot_num + 1;
    if (reboot_num % 5 == 0) {
      ESP.restart();
    }
    WiFi.begin(ssid, password);
  }
  if ((WiFi.status() == WL_CONNECTED) && (!Blynk.connected())) {
    reboot_num = reboot_num + 1;
    if ((reboot_num == 1) || (reboot_num == 2)) {
      Serial.println("...");
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(ssid, password);
    }
    if (reboot_num % 5 == 0) {
      ESP.restart();
    }
  }
  if (Blynk.connected()) {
    if (reboot_num != 0) {
      reboot_num = 0;
    }
  }
}
//------------------------
void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}
void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}
void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}
void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}
//-------------------------
void update_fw() {
  WiFiClientSecure client_;
  client_.setInsecure();
  Serial.print("Wait...");
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  t_httpUpdate_return ret = ESPhttpUpdate.update(client_, URL_fw_Bin);
  switch (ret) {
    case HTTP_UPDATE_FAILED: Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); break;
    case HTTP_UPDATE_NO_UPDATES: Serial.println("HTTP_UPDATE_NO_UPDATES"); break;
    case HTTP_UPDATE_OK: Serial.println("HTTP_UPDATE_OK"); break;
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  //---------------------------------------------------------------------------------
  emon0.current(A0, 40);
  emon1.current(A0, 40);

  pinMode(S0pin, OUTPUT);
  pinMode(S1pin, OUTPUT);
  pinMode(S2pin, OUTPUT);
  pinMode(S3pin, OUTPUT);
  pinMode(EN, OUTPUT);

  terminal.clear();
  pinMode(16, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  timer.setTimeout(15000L, []() {
    timer.setInterval(180005L, []() {
      connectionstatus();
      timer.restartTimer(timer_pre);
      timer.restartTimer(timer_tank);
    });
    timer.setInterval(2383L, []() {
      readAnalog();
      updata();
      timer.restartTimer(timer_pre);
      timer.restartTimer(timer_tank);
    });
    timer_pre = timer.setInterval(253, Pressure);
    timer.setInterval(3600000L, upinfo);
    timer_tank = timer.setInterval(301L, MeasureCmForSmoothing);
  });
}

void loop() {
  Blynk.run();
  timer.run();
}