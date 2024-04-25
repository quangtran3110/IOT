#define BLYNK_TEMPLATE_ID "TMPL6WyEmVeSK"
#define BLYNK_TEMPLATE_NAME "DHVuong"
#define BLYNK_AUTH_TOKEN "eBeqi9ZJhRK3r66cUzgdD1gp2xGxG7kS"
#define pin_status "&V27="
#define pin_G "&V28="
#define pin_mode "&V29="


#define BLYNK_FIRMWARE_VERSION "240426"

#define Main_TOKEN "Oyy7F8HDxVurrNg0QOSS6gjsCSQTsDqZ"
//const char* ssid = "net";
//const char* password = "Abcd@1234";
const char* ssid = "tram bom so 4";
const char* password = "0943950555";
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

const int pin_RL1 = P1;
const int pin_RL2 = P2;
const int pin_RL3 = P3;
const int pin_RL4 = P4;
const int pin_RL5 = P5;
const int pin_RL6 = P6;
const int pin_RL7 = P7;
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
//-------------------
#include <DallasTemperature.h>
#include <OneWire.h>
OneWire oneWire(D3);
DallasTemperature sensors(&oneWire);
float temp[1];
//-----------------------------
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/IOT/main/Arduino/DoThi/Phuong3/NTBinh/build/esp8266.esp8266.nodemcuv2/NTBinh.ino.bin"
String server_name = "http://sgp1.blynk.cloud/external/api/";
//-----------------------------
int timer_I;
int dayadjustment = -1;
bool key = false, blynk_first_connect = false, dayOfTheWeek_ = false;
bool sta_rl1 = LOW, sta_rl3 = LOW;
String num_van;
char B[50] = "";
//-----------------------------
struct Data {
  byte mode;
  byte reboot_num;
  byte save_num;
  uint32_t rl1_r, rl1_s;
  byte MonWeekDay, TuesWeekDay, WedWeekDay, ThuWeekDay, FriWeekDay, SatWeekend, SunWeekend;
} data, dataCheck;
const struct Data dataDefault = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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
                       + pin_status + 1
                       + pin_G + g;
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
    //Blynk.setProperty(V0, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
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
void on_fan() {
  sta_rl3 = HIGH;
  pcf8575_1.digitalWrite(pin_RL3, !sta_rl3);
}
void off_fan() {
  sta_rl3 = LOW;
  pcf8575_1.digitalWrite(pin_RL3, !sta_rl3);
}
void weekday_() {
  int A[7] = { data.MonWeekDay, data.TuesWeekDay, data.WedWeekDay, data.ThuWeekDay, data.FriWeekDay, data.SatWeekend, data.SunWeekend };
  memset(B, '\0', sizeof(B));
  for (int i = 0; i < 7; i++) {
    // Nếu ngày i được chọn
    if (A[i] == 1) {
      // Thêm giá trị i vào mảng A
      strcat(B, String(i + 1).c_str());
      strcat(B, ",");
    }
  }
  // Xóa ký tự cuối cùng là dấu phẩy
  B[strlen(B) - 1] = '\0';
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

  if (weekday() == 1) {
    dayadjustment = 6;  // needed for Sunday, Time library is day 1 and Blynk is day 7
  }
  if ((((weekday() + dayadjustment) == 1) && (data.MonWeekDay))
      || (((weekday() + dayadjustment) == 2) && (data.TuesWeekDay))
      || (((weekday() + dayadjustment) == 3) && (data.WedWeekDay))
      || (((weekday() + dayadjustment) == 4) && (data.ThuWeekDay))
      || (((weekday() + dayadjustment) == 5) && (data.FriWeekDay))
      || (((weekday() + dayadjustment) == 6) && (data.SatWeekend))
      || (((weekday() + dayadjustment) == 7) && (data.SunWeekend))) {
    dayOfTheWeek_ = true;
  } else dayOfTheWeek_ = false;
  if (data.mode == 1) {  // Auto
    if (dayOfTheWeek_) {
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
    } else {
      if (sta_rl1 == HIGH) off_van1();
    }
  }
}
void temperature() {  // Nhiệt độ
  sensors.requestTemperatures();
  //Serial.println(sensors.getDeviceCount());
  for (byte i = 0; i < sensors.getDeviceCount(); i++) {
    temp[i] = sensors.getTempCByIndex(i);
    Serial.println(temp[i]);
    if (temp[i] > 37 && sta_rl3 == LOW) on_fan();
    else if (temp[i] < 34 && sta_rl3 == HIGH) off_fan();
  }
  //Blynk.virtualWrite(V15, temp[1]);
  //Blynk.virtualWrite(V23, temp[0]);
}
BLYNK_WRITE(V0) {
  String dataS = param.asStr();
  if (dataS == "update") {
    update_fw();
  } else if (dataS == "rst") {
    ESP.restart();
  } else if (dataS == "m") {  //man
    data.mode = 0;
    savedata();
  } else if (dataS == "a") {  //auto
    data.mode = 1;
    savedata();
  } else if (dataS == "mode") {  //mode?
    String server_path = server_name + "batch/update?token=" + Main_TOKEN
                         + pin_mode + data.mode;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else if (dataS == "van1") {  //mode?
    num_van = "van1";
    String server_path = server_name + "batch/update?token=" + Main_TOKEN
                         + "&V4=" + data.rl1_r
                         + "&V4=" + data.rl1_s
                         + "&V4=" + tz
                         + "&V4=" + String(B);
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else if (dataS == "van1_on") {  //RL1 on
    if (data.mode == 0) on_van1();
  } else if (dataS == "van1_off") {  //RL1 off
    if (data.mode == 0) off_van1();
  }
}
BLYNK_WRITE(V1) {
  TimeInputParam t(param);
  if (num_van == "van1") {
    if (t.hasStartTime()) {
      data.rl1_r = t.getStartHour() * 3600 + t.getStartMinute() * 60;
    }
    if (t.hasStopTime()) {
      data.rl1_s = t.getStopHour() * 3600 + t.getStopMinute() * 60;
    }
  }
  data.MonWeekDay = t.isWeekdaySelected(1);
  data.TuesWeekDay = t.isWeekdaySelected(2);
  data.WedWeekDay = t.isWeekdaySelected(3);
  data.ThuWeekDay = t.isWeekdaySelected(4);
  data.FriWeekDay = t.isWeekdaySelected(5);
  data.SatWeekend = t.isWeekdaySelected(6);
  data.SunWeekend = t.isWeekdaySelected(7);
  if (memcmp(&data, &dataCheck, sizeof(dataDefault)) != 0) {
    dataCheck.MonWeekDay = data.MonWeekDay;
    dataCheck.TuesWeekDay = data.TuesWeekDay;
    dataCheck.WedWeekDay = data.WedWeekDay;
    dataCheck.ThuWeekDay = data.ThuWeekDay;
    dataCheck.FriWeekDay = data.FriWeekDay;
    dataCheck.SatWeekend = data.SatWeekend;
    dataCheck.SunWeekend = data.SunWeekend;
  }
  savedata();
  weekday_();
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
  rtc_module.begin();
  eeprom.initialize();
  eeprom.readBytes(address, sizeof(dataDefault), (byte*)&data);
  //-----------------------
  Wire.begin();
  sensors.begin();
  pcf8575_1.begin();
  delay(10000);

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
    weekday_();
    timer_I = timer.setInterval(5089, []() {
      //readcurrent();
      //readcurrent1();
      //readcurrent2();
      //readcurrent3();
      up();
      temperature();
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
