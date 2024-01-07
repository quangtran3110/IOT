#define BLYNK_TEMPLATE_ID "TMPL6VP9MY4gS"
#define BLYNK_TEMPLATE_NAME "Cau Cua Dong"
#define BLYNK_AUTH_TOKEN "jaQFoaOgdcZcKbyI_ME_oi6tThEf4FR5"
#define BLYNK_FIRMWARE_VERSION "240107"

#define Main_TOKEN "Oyy7F8HDxVurrNg0QOSS6gjsCSQTsDqZ"
const char* ssid = "net";
const char* password = "Abcd@1234";
//const char* ssid = "tram bom so 4";
//const char* password = "0943950555";
//-------------------------------------------------------------------
#define BLYNK_PRINT Serial
#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
//-----------------------------
#include "PCF8575.h"
PCF8575 pcf8575_1(0x20);
const int S0 = P15;
const int S1 = P14;
const int S2 = P13;
const int S3 = P12;

const int pin_RL1 = P7;
const int pin_RL2 = P6;
const int pin_RL3 = P5;
const int pin_RL4 = P4;
const int pin_RL5 = P3;
const int pin_RL6 = P2;
const int pin_RL7 = P1;
//-----------------------------
#include "EmonLib.h"
EnergyMonitor emon0, emon1, emon2, emon3;
int xSetAmpe = 0, xSetAmpe1 = 0, xSetAmpe2 = 0, xSetAmpe3 = 0;
unsigned long int yIrms0 = 0, yIrms1 = 0, yIrms2 = 0, yIrms3 = 0;
float Irms0, Irms1, Irms2, Irms3;
bool trip0 = false, trip1 = false, trip2 = false, trip3 = false;
float SetAmpemax = 0, SetAmpemin = 0;
//-----------------------------
#include <WidgetRTC.h>
#include "RTClib.h"
RTC_DS3231 rtc_module;
char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
char tz[] = "Asia/Ho_Chi_Minh";
//-----------------------------
#include <Wire.h>
#include <Eeprom24C32_64.h>
#define EEPROM_ADDRESS 0x57
static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);
const word address = 0;
//-----------------------------
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/IOT/main/Arduino/DoThi/Phuong2/Caucuadong/build/esp8266.esp8266.nodemcuv2/Caucuadong.ino.bin"
String server_name = "http://sgp1.blynk.cloud/external/api/";
//-----------------------------
int timer_I;
bool key = false, blynk_first_connect = false;
bool sta_rl1 = LOW;
byte num_van;
//-----------------------------
struct Data {
  byte mode;
  byte reboot_num;
  byte save_num;
  uint32_t rl1_r, rl1_s;
} data, dataCheck;
const struct Data dataDefault = { 0, 0, 0, 0, 0 };

WidgetTerminal terminal(V0);
WidgetRTC rtc_widget;
//-------------------------------------------------------------------
BlynkTimer timer;
BLYNK_CONNECTED() {
  rtc_widget.begin();
  blynk_first_connect = true;
}
//-------------------------------------------------------------------
void up() {
  byte g;
  bitWrite(g, 0, data.mode);
  bitWrite(g, 1, sta_rl1);
  String server_path = server_name + "batch/update?token=" + Main_TOKEN
                       + "&V6=" + 1
                       + "&V9=" + g;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  http.end();
}
void savedata() {
  if (memcmp(&data, &dataCheck, sizeof(dataDefault)) == 0) {
    // Serial.println("structures same no need to write to EEPROM");
  } else {
    // Serial.println("\nWrite bytes to EEPROM memory...");
    data.save_num = data.save_num + 1;
    eeprom.writeBytes(address, sizeof(dataDefault), (byte*)&data);
    Blynk.setProperty(V0, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
  }
}
void on_van1() {
  sta_rl1 = HIGH;
  pcf8575_1.digitalWrite(pin_RL1, !sta_rl1);
}
void off_van1() {
  sta_rl1 = LOW;
  pcf8575_1.digitalWrite(pin_RL1, !sta_rl1);
}
void readcurrent()  // C2 - Cấp 1   - I0
{
  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float rms0 = emon0.calcIrms(1480);
  if (rms0 < 2) {
    Irms0 = 0;
    yIrms0 = 0;
  } else if (rms0 >= 2) {
    yIrms0 = yIrms0 + 1;
    Irms0 = rms0;
    if (yIrms0 > 3) {
      if ((Irms0 >= SetAmpemax) || (Irms0 <= SetAmpemin)) {
        xSetAmpe = xSetAmpe + 1;
        if (xSetAmpe > 3) {
          off_van1();
          xSetAmpe = 0;
          trip0 = true;
          Blynk.logEvent("error", String("Van 1 lỗi: ") + Irms0 + String(" A"));
        }
      } else {
        xSetAmpe = 0;
      }
    }
  }
}
void rtctime() {
  DateTime now = rtc_module.now();
  if (blynk_first_connect == true) {
    if ((now.day() != day()) || (now.hour() != hour()) || ((now.minute() - minute() > 2) || (minute() - now.minute() > 2))) {
      rtc_module.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
      DateTime now = rtc_module.now();
    }
  }
  terminal.clear();
  //Blynk.virtualWrite(V0, daysOfTheWeek[now.dayOfTheWeek()], ", ", now.day(), "/", now.month(), "/", now.year(), " - ", now.hour(), ":", now.minute(), ":", now.second());
  //Blynk.virtualWrite(V0, "run:", data.rl1_r, ", stop:", data.rl1_s);
  float nowtime = (now.hour() * 3600 + now.minute() * 60);

  if (data.mode == 1) {  // Auto
    if (data.rl1_r > data.rl1_s) {
      if ((nowtime > data.rl1_s) && (nowtime < data.rl1_r)) {
        off_van1();
      }
      if ((nowtime < data.rl1_s) || (nowtime > data.rl1_r)) {
        on_van1();
      }
    }
    if (data.rl1_r < data.rl1_s) {
      if ((nowtime > data.rl1_s) || (nowtime < data.rl1_r)) {
        off_van1();
      }
      if ((nowtime < data.rl1_s) && (nowtime > data.rl1_r)) {
        on_van1();
      }
    }
  }
}
BLYNK_WRITE(V0) {
  String dataS = param.asStr();
  if (dataS == "update") {
    update_fw();
  } else if (dataS == "m") {  //man
    data.mode = 0;
    savedata();
  } else if (dataS == "a") {  //auto
    data.mode = 1;
    savedata();
  } else if (dataS == "mode") {  //mode?
    String server_path = server_name + "batch/update?token=" + Main_TOKEN
                         + "&V8=" + data.mode;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else if (dataS == "van1") {  //mode?
    String server_path = server_name + "batch/update?token=" + Main_TOKEN
                         + "&V4=" + data.rl1_r
                         + "&V4=" + data.rl1_s
                         + "&V4=" + tz;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else if (dataS == "van1_on") {  //RL1 on
    on_van1();
  } else if (dataS == "van1_off") {  //RL1 off
    off_van1();
  }
}
BLYNK_WRITE(V1) {
  TimeInputParam t(param);
  if (num_van == 1) {
    if (t.hasStartTime()) {
      data.rl1_r = t.getStartHour() * 3600 + t.getStartMinute() * 60;
    }
    if (t.hasStopTime()) {
      data.rl1_s = t.getStopHour() * 3600 + t.getStopMinute() * 60;
    }
  }
  savedata();
}
BLYNK_WRITE(V2) {
  num_van = param.asInt();
}
//-------------------------
void connectionstatus() {
  if ((WiFi.status() != WL_CONNECTED)) {
    //Serial.println("Khong ket noi WIFI");
  }
  if ((WiFi.status() == WL_CONNECTED) && (!Blynk.connected())) {
    data.reboot_num = data.reboot_num + 1;
    savedata();
    if ((data.reboot_num == 1) || (data.reboot_num == 2)) {
      delay(1000);
      ESP.restart();
    }
    if (data.reboot_num % 5 == 0) {
      delay(1000);
      ESP.restart();
    }
  }
  if (Blynk.connected()) {
    if (data.reboot_num != 0) {
      data.reboot_num = 0;
      savedata();
    }
  }
}
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
//-------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  //-----------------------
  delay(10000);
  //-----------------------
  rtc_module.begin();
  eeprom.initialize();
  eeprom.readBytes(address, sizeof(dataDefault), (byte*)&data);
  //-----------------------
  emon0.current(A0, 110);
  //emon1.current(A0, 110);
  //emon2.current(A0, 110);
  //emon3.current(A0, 110);
  //-----------------------
  Wire.begin();
  pcf8575_1.begin();
  pcf8575_1.pinMode(S0, OUTPUT);
  pcf8575_1.pinMode(S1, OUTPUT);
  pcf8575_1.pinMode(S2, OUTPUT);
  pcf8575_1.pinMode(S3, OUTPUT);
  pcf8575_1.pinMode(pin_RL1, OUTPUT);
  pcf8575_1.digitalWrite(pin_RL1, HIGH);
  pcf8575_1.pinMode(pin_RL2, OUTPUT);
  pcf8575_1.digitalWrite(pin_RL2, HIGH);
  pcf8575_1.pinMode(pin_RL3, OUTPUT);
  pcf8575_1.digitalWrite(pin_RL3, HIGH);
  pcf8575_1.pinMode(pin_RL4, OUTPUT);
  pcf8575_1.digitalWrite(pin_RL4, HIGH);
  pcf8575_1.pinMode(pin_RL5, OUTPUT);
  pcf8575_1.digitalWrite(pin_RL5, HIGH);
  pcf8575_1.pinMode(pin_RL6, OUTPUT);
  pcf8575_1.digitalWrite(pin_RL6, HIGH);
  pcf8575_1.pinMode(pin_RL7, OUTPUT);
  pcf8575_1.digitalWrite(pin_RL7, HIGH);

  timer.setTimeout(5000L, []() {
    timer_I = timer.setInterval(5089, []() {
      readcurrent();
      //readcurrent1();
      //readcurrent2();
      //readcurrent3();
      up();
      timer.restartTimer(timer_I);
    });
    timer.setInterval(15005L, []() {
      rtctime();
      timer.restartTimer(timer_I);
    });
    timer.setInterval(900005L, []() {
      connectionstatus();
      timer.restartTimer(timer_I);
    });
  });
}
void loop() {
  Blynk.run();
  timer.run();
}
