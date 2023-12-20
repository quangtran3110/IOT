/*V0 - Button C2-1
 *V1 - Button C2-2
 *V2 - Button C1
 *V3 - Switch - chế độ Pump
 *V4 - Chọn người vận hành
 *V5 - MENU motor
 *V6 - min
 *V7 - max
 *V8 -
 *V9 - Ngày/Giờ
 *V10 - terminal key
 *V11 - Thời gian chạy Bơm
 *V12 -
 *V13 - Bảo vệ
 *V14 - Ap luc
 *V15 - Nhiet do dong co 1
 *V16 - Thông báo
 *V17 - Van điện Rửa lọc
 *V18 - time input
 *V19 - Thể tích

 *V20 - haohut
 *V21 - Do Sau
 *V22 - Dung Tich
 *V23 - Nhiệt độ động cơ 2
 *V24 - I2 - Bơm 2
 *V25 - I4 - Van điện rửa lọc
 *V26 - I1 - Bơm 1
 *V27 - I0 - Giếng
 *V28 - Hide/visible_all
 *V29 - Info
 *V30 - I3 - Nén khí
 *V31 - Terminal Luu luong
 *V32 - input luu luong
 *V33 - check luu luong
 *V34 - menu chọn giếng
 *V35 - Luu Luong G1
 *V36 - Khoi luong Clo
 *V37 - Luu Luong G1 trong ngay
 *V40 -
 *V41 -
 *V42 -
 *V43 -

 *MCP------
 *pin8 - B0 - nối relay 1
 *pin9 - B1 - nối relay 2
 *pin10 - B2 - nối relay 3
 *
 *pin0 - A0 - tín hiệu kích R1
 *pin1 - A1 - tín hiệu kích R2
 *pin2 - A2 - tín hiệu kích R3
 *
 *pin12 - B4 - tín hiệu chạy/tắt
 *pin13 - B5 - tín hiệu chạy/tắt
 *pin14 - B6 - tín hiệu chạy/tắt
 *
 */

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLJp_sN4GN"
#define BLYNK_TEMPLATE_NAME "Trạm Số 4"
#define BLYNK_AUTH_TOKEN "o-H-k28kNBIzgNIAP89f2AElv--eWuVO"
#define BLYNK_FIRMWARE_VERSION "231220.T4.MAIN"
#define APP_DEBUG

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#include <Adafruit_MCP23X17.h>

#include "EmonLib.h"

#include <WidgetRTC.h>
#include "RTClib.h"
RTC_DS3231 rtc_module;

#include <Wire.h>
#include <Eeprom24C32_64.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

WiFiClient client;
HTTPClient http;
#define URL_fw_Bin "https://github.com/quangtran3110/IOT/raw/main/Arduino/Tram4/main/build/esp8266.esp8266.nodemcuv2/main.ino.bin"

String server_name = "http://sgp1.blynk.cloud/external/api/";
String Main = "o-H-k28kNBIzgNIAP89f2AElv--eWuVO";

#define filterSamples 121
#define EEPROM_ADDRESS 0x57
static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);

OneWire oneWire(D3);
DallasTemperature sensors(&oneWire);

Adafruit_MCP23X17 mcp;
EnergyMonitor emon0, emon1, emon2, emon3, emon4;

const char* ssid = "tram bom so 4";
const char* password = "0943950555";

const int S0pin = 14;
const int S1pin = 12;
const int S2pin = 13;
const int S3pin = 15;
const word address = 0;

const int pincap1 = 14;
const int pinbom1 = 13;
const int pinbom2 = 9;
const int pinvandien = 11;

int dai = 800;
int rong = 800;
int dosau = 330;
int volume, volume1, percent, percent1, dungtich, smoothDistance;
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

  for (j = 0; j < filterSamples; j++)  // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];

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
long distance, distance1, t;
long m = 60 * 1000;

char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
char tz[] = "Asia/Ho_Chi_Minh";
bool key = false, keySet = false, keyp = true, keynoti = true, keytank = true;
bool trip0 = false, trip1 = false, trip2 = false, trip3 = false, trip4 = false;
bool timer_updata_status, timer_I_status, key_memory = true;
bool time_run1 = false, time_run2 = false;
bool noti_1 = true, noti_2 = true, noti_3 = true, noti_4 = true, noti_5 = true, noti_6 = true;
bool blynk_first_connect = false;

byte menu_gieng_luuluong;
unsigned long ll_g_cache = 0;
float clo_cache = 0;
uint32_t timestamp;

int a, c, b, f = 0, check_connect = 0;
int timer_2, timer_1, timer_3, timer_4, timer_5;
int RelayState = LOW, RelayState2 = HIGH, RelayState1 = LOW;
int xSetAmpe = 0, xSetAmpe1 = 0, xSetAmpe2 = 0, xSetAmpe3 = 0, xSetAmpe4 = 0;
int btnState = HIGH, btnState1 = HIGH, btnState2 = HIGH, AppState, AppState1, AppState2;
int var_LLG;
float Irms0, Irms1, Irms2, Irms3, Irms4, value, Result1, temp[3];
unsigned long int xIrms0 = 0, xIrms1 = 0, xIrms2 = 0, xIrms3 = 0, xIrms4 = 0;
unsigned long int yIrms0 = 0, yIrms1 = 0, yIrms2 = 0, yIrms3 = 0, yIrms4 = 0;
unsigned long int d = 0;
String s;
struct Data {
  byte SetAmpemax, SetAmpemin;
  byte SetAmpe1max, SetAmpe1min;
  byte SetAmpe2max, SetAmpe2min;
  byte SetAmpe3max, SetAmpe3min;
  byte SetAmpe4max, SetAmpe4min;
  byte man, mode_cap2, cap2_chanle;
  int b1_1_start, b1_1_stop, b1_2_start, b1_2_stop, b1_3_start, b1_3_stop, b1_4_start, b1_4_stop;
  int b2_1_start, b2_1_stop, b2_2_start, b2_2_stop, b2_3_start, b2_3_stop, b2_4_start, b2_4_stop;
  int bom_chanle_start, bom_chanle_stop;
  byte reboot_num;
  int save_num;
  int time_run_nenkhi, time_stop_nenkhi;
  float clo;
  int time_clo, LL_RL;
  byte statusRualoc;
} data, dataCheck;
const struct Data dataDefault = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

WidgetTerminal terminal(V10);
WidgetTerminal terminal_luuluong(V31);
WidgetRTC rtc_widget;

BlynkTimer timer, timer1;
BLYNK_CONNECTED() {
  rtc_widget.begin();
  blynk_first_connect = true;
  Blynk.setProperty(V10, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
}
//-------------------------------------------------------------------
void up() {
  String server_path = server_name + "batch/update?token=" + Main
                       //+ "&V0=" + byte(RelayState)
                       //+ "&V1=" + byte(RelayState1)
                       //+ "&V2=" + byte(RelayState2)
                       + "&V14=" + float(Result1)
                       //+ "&V15=" + temp[1]
                       + "&V19=" + volume1
                       + "&V20=" + smoothDistance
                       //+ "&V23=" + temp[0]
                       + "&V27=" + Irms0
                       + "&V26=" + Irms1
                       + "&V24=" + Irms2
                       + "&V30=" + Irms3
                       + "&V25=" + Irms4;
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
    Blynk.setProperty(V10, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
  }
}
void rualoc() {
  if (data.statusRualoc == HIGH) {
    mcp.digitalWrite(pinvandien, !data.statusRualoc);
    timer1.setTimeout(long(data.time_run_nenkhi * 1000), []() {
      mcp.digitalWrite(pinvandien, HIGH);
    });
  }
}
void oncap1() {
  RelayState2 = HIGH;
  mcp.digitalWrite(pincap1, RelayState2);
  Blynk.virtualWrite(V2, RelayState2);
}
void offcap1() {
  RelayState2 = LOW;
  mcp.digitalWrite(pincap1, RelayState2);
  Blynk.virtualWrite(V2, RelayState2);
}
void onbom1() {
  RelayState = HIGH;
  mcp.digitalWrite(pinbom1, !RelayState);
  Blynk.virtualWrite(V0, RelayState);
}
void offbom1() {
  RelayState = LOW;
  mcp.digitalWrite(pinbom1, !RelayState);
  Blynk.virtualWrite(V0, RelayState);
}
void onbom2() {
  RelayState1 = HIGH;
  mcp.digitalWrite(pinbom2, !RelayState1);
  Blynk.virtualWrite(V1, RelayState1);
}
void offbom2() {
  RelayState1 = LOW;
  mcp.digitalWrite(pinbom2, !RelayState1);
  Blynk.virtualWrite(V1, RelayState1);
}
void hidden_all() {
  Blynk.setProperty(V3, "isHidden", true);
  Blynk.setProperty(V18, "isHidden", true);
  Blynk.setProperty(V11, "isHidden", true);
  Blynk.setProperty(V16, "isHidden", true);
  Blynk.setProperty(V13, "isHidden", true);
  Blynk.setProperty(V8, "isHidden", true);
  Blynk.setProperty(V5, "isHidden", true);
  Blynk.setProperty(V6, "isHidden", true);
  Blynk.setProperty(V7, "isHidden", true);
}
void visible_all() {
  Blynk.setProperty(V3, "isHidden", false);
  Blynk.setProperty(V18, "isHidden", false);
  Blynk.setProperty(V11, "isHidden", false);
  Blynk.setProperty(V16, "isHidden", false);
  Blynk.setProperty(V13, "isHidden", false);
  Blynk.setProperty(V8, "isHidden", false);
  Blynk.setProperty(V5, "isHidden", false);
  Blynk.setProperty(V6, "isHidden", false);
  Blynk.setProperty(V7, "isHidden", false);
}
void hidden_auto() {
  Blynk.setProperty(V0, "isHidden", true);
  Blynk.setProperty(V1, "isHidden", true);
  Blynk.setProperty(V2, "isHidden", true);
}
void visible_man() {
  Blynk.setProperty(V0, "isHidden", false);
  Blynk.setProperty(V1, "isHidden", false);
  Blynk.setProperty(V2, "isHidden", false);
}
void syncstatus() {
  //Blynk.virtualWrite(V0, RelayState);
  //Blynk.virtualWrite(V1, RelayState1);
  //Blynk.virtualWrite(V2, RelayState2);
  if (data.mode_cap2 == 3)  // Chạy 2 bơm
    hidden_auto();
  else if (data.mode_cap2 == 0)
    visible_man();
  //Blynk.virtualWrite(V17, statusRualoc);
}
void readPower()  // C2 - Giếng    - I0
{
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float rms0 = emon0.calcIrms(1480);
  if (rms0 < 2) {
    Irms0 = 0;
    yIrms0 = 0;
  } else if (rms0 >= 2) {
    Irms0 = rms0;
    yIrms0 = yIrms0 + 1;
    if ((yIrms0 > 3) && ((Irms0 >= data.SetAmpemax) || (Irms0 <= data.SetAmpemin))) {
      xSetAmpe = xSetAmpe + 1;
      if ((xSetAmpe >= 4) && (keyp)) {
        offcap1();
        xSetAmpe = 0;
        trip0 = true;
        if (keynoti)
          Blynk.logEvent("error", String("Bơm GIẾNG lỗi: ") + Irms0 + String(" A"));
      }
    }
  }
  //Blynk.virtualWrite(V27, Irms0);
}
void readPower1()  // C1 - Bơm 1    - I1
{
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float rms1 = emon1.calcIrms(1480);
  if (rms1 < 1) {
    Irms1 = 0;
    yIrms1 = 0;
  } else if (rms1 >= 1) {
    Irms1 = rms1;
    yIrms1 = yIrms1 + 1;
    if ((yIrms1 > 3) && ((Irms1 >= data.SetAmpe1max) || (Irms1 <= data.SetAmpe1min))) {
      xSetAmpe1 = xSetAmpe1 + 1;
      if ((xSetAmpe1 >= 2) && (keyp)) {
        offbom1();
        xSetAmpe1 = 0;
        trip1 = true;
        if (keynoti)
          Blynk.logEvent("error", String("Bơm 1 lỗi: ") + Irms1 + String(" A"));
      }
    }
  }
  //Blynk.virtualWrite(V26, Irms1);
}
void readPower2()  // C3 - Bơm 2    - I2
{
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float rms2 = emon2.calcIrms(1480);
  if (rms2 < 1) {
    Irms2 = 0;
    yIrms2 = 0;
  } else if (rms2 >= 1) {
    Irms2 = rms2;
    yIrms2 = yIrms2 + 1;
    if ((yIrms2 > 3) && ((Irms2 >= data.SetAmpe2max) || (Irms2 <= data.SetAmpe2min))) {
      xSetAmpe2 = xSetAmpe2 + 1;
      if ((xSetAmpe2 >= 2) && (keyp)) {
        offbom2();
        xSetAmpe2 = 0;
        trip2 = true;
        if (keynoti)
          Blynk.logEvent("error", String("Bơm 2 lỗi: ") + Irms2 + String(" A"));
      }
    }
  }
  //Blynk.virtualWrite(V24, Irms2);
}
void readPower3()  // C4 - Nén khí  - I3
{
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  float rms3 = emon3.calcIrms(740);
  if (rms3 < 1) {
    Irms3 = 0;
    yIrms3 = 0;
  } else if (rms3 >= 1) {
    Irms3 = rms3;
    yIrms3 = yIrms3 + 1;
    if ((yIrms3 > 3) && ((Irms3 >= data.SetAmpe3max) || (Irms3 <= data.SetAmpe3min))) {
      xSetAmpe3 = xSetAmpe3 + 1;
      if ((xSetAmpe3 >= 3) && (keyp)) {
        offcap1();
        trip3 = true;
        xSetAmpe3 = 0;
        if (keynoti)
          Blynk.logEvent("error", String("Máy NÉN KHÍ lỗi: ") + Irms3 + String(" A"));
      }
    } else {
      xSetAmpe3 = 0;
    }
  }
  //Blynk.virtualWrite(V30, Irms3);
}
void readPower4()  // C0 - Van điện - I4
{
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float rms4 = emon4.calcIrms(740);
  if (rms4 < 1) {
    Irms4 = 0;
    yIrms4 = 0;
  } else if (rms4 >= 1) {
    Irms4 = rms4;
    yIrms4 = yIrms4 + 1;
    if ((yIrms4 > 3) && ((Irms4 >= data.SetAmpe4max) || (Irms4 <= data.SetAmpe4min))) {
      xSetAmpe4 = xSetAmpe4 + 1;
      if ((xSetAmpe4 >= 2) && (keyp)) {
        data.statusRualoc = LOW;
        savedata();
        mcp.digitalWrite(pinvandien, !data.statusRualoc);
        xSetAmpe4 = 0;
        trip4 = true;
        if (keynoti)
          Blynk.logEvent("error", String("Van điện lỗi: ") + Irms4 + String(" A"));
      }
    } else {
      xSetAmpe4 = 0;
    }
  }
  //Blynk.virtualWrite(V25, Irms4);
}
void temperature() {  // Nhiệt độ
  sensors.requestTemperatures();
  //Serial.println(sensors.getDeviceCount());
  for (byte i = 0; i < sensors.getDeviceCount(); i++)
    temp[i] = sensors.getTempCByIndex(i);
  //Blynk.virtualWrite(V15, temp[1]);
  //Blynk.virtualWrite(V23, temp[0]);
}
//-------------------------------------------------------------------
void readPressure()  // C6 - Ap Luc
{
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  float sensorValue = analogRead(A0);
  float Result;
  Result = (((sensorValue - 186) * 6) / (930 - 186));
  if (Result > 0) {
    value += Result;
    Result1 = value / 16.0;
    value -= Result1;
  }
}
//-------------------------------------------------------------------
void MeasureCmForSmoothing() {
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  float sensorValue = analogRead(A0);
  distance1 = (((sensorValue - 196.5) * 500) / (950 - 196.5));  // 915,74 (R=147.7)
  if (distance1 > 0) {
    smoothDistance = digitalSmooth(distance1, sensSmoothArray1);
    volume1 = (dai * smoothDistance * rong) / 1000000;
    if ((smoothDistance < (dosau / 2)) && (Irms3 == 0) && !trip3 && keynoti && keytank) {
      Blynk.logEvent("info", String("Mực nước thấp nhưng cấp 1 không chạy: ") + smoothDistance + String(" cm"));
      keytank = false;
      timer1.setTimeout(10 * m, []() {
        keytank = true;
      });
    } else if ((smoothDistance - dosau >= 20) && (keynoti) && (keytank)) {
      Blynk.logEvent("info", String("Nước trong bể cao vượt mức ") + (smoothDistance - dosau) + String(" cm"));
      keytank = false;
      timer1.setTimeout(15 * m, []() {
        keytank = true;
      });
    } else keytank = true;
  }
}
//-------------------------------------------------------------------
void rtctime() {
  DateTime now = rtc_module.now();
  if (blynk_first_connect == true) {
    if ((now.day() != day()) || (now.hour() != hour()) || ((now.minute() - minute() > 2) || (minute() - now.minute() > 2))) {
      rtc_module.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
      DateTime now = rtc_module.now();
    }
  }
  timestamp = now.unixtime();
  Blynk.virtualWrite(V9, daysOfTheWeek[now.dayOfTheWeek()], ", ", now.day(), "/", now.month(), "/", now.year(), " - ", now.hour(), ":", now.minute(), ":", now.second());

  int nowtime = (now.hour() * 3600 + now.minute() * 60);
  if (data.mode_cap2 == 3) {                                                                                                                                                                                                                         // Chọn chế độ chạy 2 bơm
    if ((nowtime > data.b1_1_start && nowtime < data.b1_1_stop) || (nowtime > data.b1_2_start && nowtime < data.b1_2_stop) || (nowtime > data.b1_3_start && nowtime < data.b1_3_stop) || (nowtime > data.b1_4_start && nowtime < data.b1_4_stop)) {  // Chạy bơm 1
      if (Irms1 == 0 && !trip1) {                                                                                                                                                                                                                    // Nếu bơm 1 đang tắt và không lỗi
        if ((Irms2 == 0 && !time_run2) || (time_run2)) onbom1();                                                                                                                                                                                     //Chạy bơm 1
        if (time_run1 && noti_3) {
          noti_3 = false;
          if (keynoti) Blynk.logEvent("error", String("LỖI: Bơm 1 không chạy."));
          timer1.setTimeout(30 * m, []() {
            noti_3 = true;
          });
        }
      }
      time_run1 = true;
    } else {
      if (Irms1 != 0) {
        offbom1();
        if (!time_run1 && noti_1) {
          noti_1 = false;
          if (keynoti) Blynk.logEvent("error", String("Lỗi lịch chạy!\nBơm 1 đang chạy: ") + Irms1 + String(" A"));
          timer1.setTimeout(5 * m, []() {
            noti_1 = true;
          });
        }
      }
      time_run1 = false;
    }
    if ((nowtime > data.b2_1_start && nowtime < data.b2_1_stop) || (nowtime > data.b2_2_start && nowtime < data.b2_2_stop) || (nowtime > data.b2_3_start && nowtime < data.b2_3_stop) || (nowtime > data.b2_4_start && nowtime < data.b2_4_stop)) {
      if (Irms2 == 0 && !trip2) {                                 // Nếu bơm 2 đang tắt và không lỗi
        if ((Irms1 == 0 && !time_run1) || (time_run1)) onbom2();  //Chạy bơm 2
        if (time_run2 && noti_4) {
          noti_4 = false;
          if (keynoti) Blynk.logEvent("error", String("LỖI: Bơm 2 không chạy."));
          timer1.setTimeout(30 * m, []() {
            noti_4 = true;
          });
        }
      }
      time_run2 = true;
    } else {
      if (Irms2 != 0) {
        offbom2();
        if (!time_run2 && noti_2) {
          noti_2 = false;
          if (keynoti) Blynk.logEvent("error", String("Lỗi lịch chạy!\nBơm 2 đang chạy: ") + Irms2 + String(" A"));
          timer1.setTimeout(5 * m, []() {
            noti_2 = true;
          });
        }
      }
      time_run2 = false;
    }
  } else if (data.mode_cap2 == 1) {  // Chạy bơm 1
    if (Irms0 != 0) {                // Tắt Bơm 2
      offbom2();
      timer1.setTimeout(3000L, []() {
        if (Irms0 != 0) {
          offbom2();
          if (keynoti) Blynk.logEvent("error", String("Lỗi lịch chạy!\nBơm 2 đang chạy: ") + Irms0 + String(" A"));
        }
      });
    }
    if (data.cap2_chanle == 0) {  // Chọn ngày chẵn tắt máy
      if (now.day() % 2 == 0) {
        if ((nowtime > data.bom_chanle_stop) && (Irms2 != 0))
          offbom1();
        if ((nowtime < data.bom_chanle_stop) && (trip2 == false) && (Irms2 == 0)) {
          onbom1();
          if (Irms2 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 1 bị lỗi không chạy kìa.\nKiểm tra lẹ."));
          }
        }
      }
      if (now.day() % 2 != 0) {
        if (((nowtime >= data.bom_chanle_start) && (trip2 == false)) && (Irms2 == 0)) {
          onbom1();
          if (Irms2 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 1 lỗi không thể chạy.\nXin hãy kiểm tra. "));
          }
        }
      }
    }
    if (data.cap2_chanle == 1) {  // Chon ngay le tat may
      if (now.day() % 2 != 0) {
        if ((nowtime > data.bom_chanle_stop) && (Irms2 != 0))
          offbom1();
        if ((nowtime < data.bom_chanle_stop) && (trip2 == false) && (Irms2 == 0)) {
          onbom1();
          if (Irms2 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 1 lỗi không thể chạy.\nXin hãy kiểm tra. "));
          }
        }
      }
      if (now.day() % 2 == 0) {
        if (((nowtime >= data.bom_chanle_start) && (trip2 == false)) && (Irms2 == 0)) {
          onbom1();
          if (Irms2 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 1 lỗi không thể chạy.\nXin hãy kiểm tra. "));
          }
        }
      }
    }
  } else if (data.mode_cap2 == 2) {  // Chạy bơm 2
    if (Irms2 != 0) {                // Tắt Bơm 1
      offbom1();
      timer1.setTimeout(3000L, []() {
        if (Irms2 != 0) {
          offbom1();
          if (keynoti) Blynk.logEvent("error", String("Lỗi lịch chạy!\nBơm 1 đang chạy: ") + Irms2 + String(" A"));
        }
      });
    }
    if (data.cap2_chanle == 0) {  // Chọn ngày chẵn tắt máy
      if (now.day() % 2 == 0) {
        if ((nowtime > data.bom_chanle_stop) && (Irms0 != 0))
          offbom2();
        if ((nowtime < data.bom_chanle_stop) && (trip0 == false) && (Irms0 == 0)) {
          onbom2();
          if (Irms0 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 2 bị lỗi không chạy kìa.\nKiểm tra lẹ."));
          }
        }
      }
      if (now.day() % 2 != 0) {
        if (((nowtime >= data.bom_chanle_start) && (trip0 == false)) && (Irms0 == 0)) {
          onbom2();
          if (Irms0 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 2 lỗi không thể chạy.\nXin hãy kiểm tra. "));
          }
        }
      }
    }
    if (data.cap2_chanle == 1) {  // Chon ngay le tat may
      if (now.day() % 2 != 0) {
        if ((nowtime > data.bom_chanle_stop) && (Irms0 != 0))
          offbom2();
        if ((nowtime < data.bom_chanle_stop) && (trip0 == false) && (Irms0 == 0)) {
          onbom2();
          if (Irms0 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 2 lỗi không thể chạy.\nXin hãy kiểm tra. "));
          }
        }
      }
      if (now.day() % 2 == 0) {
        if (((nowtime >= data.bom_chanle_start) && (trip0 == false)) && (Irms0 == 0)) {
          onbom2();
          if (Irms0 == 0) {
            f = f + 1;
            if ((f == 3) && (keynoti))
              Blynk.logEvent("error", String("Bơm 2 lỗi không thể chạy.\nXin hãy kiểm tra. "));
          }
        }
      }
    }
  }
}
//-------------------------------------------------------------------
BLYNK_WRITE(V0)  // Bơm 1
{
  if (key && !trip2)
    if (param.asInt() == LOW)
      offbom1();
    else onbom1();
  else
    Blynk.virtualWrite(V0, RelayState);
}
BLYNK_WRITE(V1)  // Bơm 2
{
  if (key && !trip0)
    if (param.asInt() == LOW)
      offbom2();
    else onbom2();
  else
    Blynk.virtualWrite(V1, RelayState1);
}
BLYNK_WRITE(V2)  // Giếng
{
  if (key && !trip3)
    if (param.asInt() == LOW)
      offcap1();
    else oncap1();
  else
    Blynk.virtualWrite(V2, RelayState2);
}
BLYNK_WRITE(V3)  // Chọn chế độ Cấp 2
{
  if (keySet) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          data.mode_cap2 = 0;
          visible_man();
          break;
        }
      case 1:
        {  // Bơm 1
          data.mode_cap2 = 1;
          break;
        }
      case 2:
        {  // Bơm 2
          data.mode_cap2 = 2;
          break;
        }
      case 3:
        {  // Bơm 1 + 2
          data.mode_cap2 = 3;
          hidden_auto();
          break;
        }
    }
  } else
    Blynk.virtualWrite(V3, data.mode_cap2);
}
BLYNK_WRITE(V4)  // Chọn người trực
{
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Quang
          data.man = 0;
          break;
        }
      case 1:
        {  // GUEST
          data.man = 1;
          break;
        }
    }
  } else
    Blynk.virtualWrite(V4, data.man);
}
BLYNK_WRITE(V5)  // Chon máy cài đặt bảo vệ
{
  switch (param.asInt()) {
    case 0:
      {  // Cấp 1
        c = 0;
        Blynk.virtualWrite(V6, data.SetAmpemin);
        Blynk.virtualWrite(V7, data.SetAmpemax);
        break;
      }
    case 1:
      {  // Cap 2 - 1
        c = 1;
        Blynk.virtualWrite(V6, data.SetAmpe1min);
        Blynk.virtualWrite(V7, data.SetAmpe1max);
        break;
      }
    case 2:
      {  // Cap 2 - 2
        c = 2;
        Blynk.virtualWrite(V6, data.SetAmpe2min);
        Blynk.virtualWrite(V7, data.SetAmpe2max);
        break;
      }
    case 3:
      {  // Clo
        c = 3;
        Blynk.virtualWrite(V6, data.SetAmpe3min);
        Blynk.virtualWrite(V7, data.SetAmpe3max);
        break;
      }
    case 4:
      {  // NK
        c = 4;
        Blynk.virtualWrite(V6, data.SetAmpe4min);
        Blynk.virtualWrite(V7, data.SetAmpe4max);
        break;
      }
  }
}
BLYNK_WRITE(V6)  // min
{
  if (keySet) {
    if (c == 0)  //Giếng
      data.SetAmpemin = param.asInt();
    else if (c == 1)  //Bơm 1
      data.SetAmpe1min = param.asInt();
    else if (c == 2)  //Bơm 2
      data.SetAmpe2min = param.asInt();
    else if (c == 3)  //Nén khí
      data.SetAmpe3min = param.asInt();
    else if (c == 4)  //Van điện
      data.SetAmpe4min = param.asInt();
  } else {
    Blynk.virtualWrite(V6, 0);
  }
}
BLYNK_WRITE(V7)  // max
{
  if (keySet) {
    if (c == 0)  //Giếng
      data.SetAmpemax = param.asInt();
    else if (c == 1)  //Bơm 1
      data.SetAmpe1max = param.asInt();
    else if (c == 2)  //Bơm 2
      data.SetAmpe2max = param.asInt();
    else if (c == 3)  //Nén khí
      data.SetAmpe3max = param.asInt();
    else if (c == 4)  //Van điện
      data.SetAmpe4max = param.asInt();
  } else {
    Blynk.virtualWrite(V7, 0);
  }
}

BLYNK_WRITE(V10)  // String
{
  String dataS = param.asStr();
  if (dataS == "M") {
    terminal.clear();
    key = true;
    Blynk.virtualWrite(V10, "Người vận hành: 'M.Quang'\nKích hoạt trong 10s\n");
    timer1.setTimeout(10000L, []() {
      key = false;
      terminal.clear();
    });
  } else if (dataS == "active") {
    terminal.clear();
    key = true;
    keySet = true;
    Blynk.virtualWrite(V10, "KHÔNG sử dụng phần mềm cho đến khi thông báo này mất.\n");
  } else if (dataS == "deactive") {
    terminal.clear();
    key = false;
    keySet = false;
    Blynk.virtualWrite(V10, "Ok!\nNhập mã để điều khiển!\n");
  } else if (dataS == "save") {
    terminal.clear();
    savedata();
    Blynk.virtualWrite(V10, "Đã lưu cài đặt.\n");
  } else if (dataS == "reset") {
    terminal.clear();
    trip0 = false;
    trip1 = false;
    trip2 = false;
    trip3 = false;
    trip4 = false;
    mcp.digitalWrite(pincap1, HIGH);
    Blynk.virtualWrite(V10, "Đã RESET! \nNhập mã để điều khiển!\n");
  } else if (dataS == "save_num") {
    terminal.clear();
    Blynk.virtualWrite(V10, "Số lần ghi EEPROM: ", data.save_num);
  } else if (dataS == "rst") {
    terminal.clear();
    Blynk.virtualWrite(V10, "ESP khởi động lại sau 3s");
    delay(3000);
    ESP.restart();
  } else if (dataS == "update") {
    terminal.clear();
    Blynk.virtualWrite(V10, "UPDATE FIRMWARE...");
    update_fw();
  } else {
    Blynk.virtualWrite(V10, "Mật mã sai.\nVui lòng nhập lại!\n");
  }
}
BLYNK_WRITE(V11)  // Chọn thời gian chạy 2 Bơm
{
  if ((data.mode_cap2 == 1) || (data.mode_cap2 == 2)) {
    BlynkParamAllocated menu(128);  // list length, in bytes
    menu.add("NGÀY CHẴN");
    menu.add("NGÀY LẺ");
    menu.add("THỜI GIAN RỬA LỌC");
    Blynk.setProperty(V11, "labels", menu);
    switch (param.asInt()) {
      case 0:
        {
          if (keySet) {
            data.cap2_chanle = 0;
            b = 8;
          }
          Blynk.virtualWrite(V18, data.bom_chanle_start, data.bom_chanle_stop, tz);
          break;
        }
      case 1:
        {
          if (keySet) {
            data.cap2_chanle = 1;
            b = 8;
          }
          Blynk.virtualWrite(V18, data.bom_chanle_start, data.bom_chanle_stop, tz);
          break;
        }
      case 2:
        {  // Rửa lọc
          if (keySet)
            b = 9;
          Blynk.virtualWrite(V18, data.time_run_nenkhi, data.time_stop_nenkhi, tz);
          break;
        }
    }
  } else if (data.mode_cap2 == 3) {
    BlynkParamAllocated menu(255);  // list length, in bytes
    menu.add("BƠM 1 - LẦN 1");
    menu.add("BƠM 2 - LẦN 1");
    menu.add("BƠM 1 - LẦN 2");
    menu.add("BƠM 2 - LẦN 2");
    menu.add("BƠM 1 - LẦN 3");
    menu.add("BƠM 2 - LẦN 3");
    menu.add("BƠM 1 - LẦN 4");
    menu.add("BƠM 2 - LẦN 4");
    menu.add("THỜI GIAN RỬA LỌC");
    Blynk.setProperty(V11, "labels", menu);
    switch (param.asInt()) {
      case 0:
        {  // Bơm 1 - Lần 1
          if (keySet)
            b = 0;
          Blynk.virtualWrite(V18, data.b1_1_start, data.b1_1_stop, tz);
          break;
        }
      case 1:
        {  // Bơm 2 - Lần 1
          if (keySet)
            b = 1;
          Blynk.virtualWrite(V18, data.b2_1_start, data.b2_1_stop, tz);
          break;
        }
      case 2:
        {  // Bơm 1 - Lần 2
          if (keySet)
            b = 2;
          Blynk.virtualWrite(V18, data.b1_2_start, data.b1_2_stop, tz);
          break;
        }
      case 3:
        {  // Bơm 2 - Lần 2
          if (keySet)
            b = 3;
          Blynk.virtualWrite(V18, data.b2_2_start, data.b2_2_stop, tz);
          break;
        }
      case 4:
        {  // Bơm 1 - Lần 3
          if (keySet)
            b = 4;
          Blynk.virtualWrite(V18, data.b1_3_start, data.b1_3_stop, tz);
          break;
        }
      case 5:
        {  // Bơm 2 - Lần 3
          if (keySet)
            b = 5;
          Blynk.virtualWrite(V18, data.b2_3_start, data.b2_3_stop, tz);
          break;
        }
      case 6:
        {  // Bơm 1 - Lần 4
          if (keySet)
            b = 6;
          Blynk.virtualWrite(V18, data.b1_4_start, data.b1_4_stop, tz);
          break;
        }
      case 7:
        {  // Bơm 2 - Lần 4
          if (keySet)
            b = 7;
          Blynk.virtualWrite(V18, data.b2_4_start, data.b2_4_stop, tz);
          break;
        }
      case 8:
        {  // Châm Clo
          if (keySet)
            b = 9;
          Blynk.virtualWrite(V18, data.time_run_nenkhi, data.time_stop_nenkhi, tz);
          break;
        }
    }
  }
}
BLYNK_WRITE(V13)  // Bảo vệ
{
  if (keySet) {
    int data13 = param.asInt();
    if (data13 == LOW)
      keyp = false;
    else
      keyp = true;
  } else
    Blynk.virtualWrite(V13, keyp);
}
BLYNK_WRITE(V16)  // Thông báo
{
  if (keySet) {
    int data16 = param.asInt();
    if (data16 == LOW)
      keynoti = false;
    else
      keynoti = true;
  } else
    Blynk.virtualWrite(V16, keynoti);
}
BLYNK_WRITE(V17)  // Chế độ rửa lọc
{
  if (key) {
    if (param.asInt() == LOW) {
      data.statusRualoc = LOW;
      mcp.digitalWrite(pinvandien, !data.statusRualoc);
      if (data.LL_RL != 0) {
        Blynk.virtualWrite(V38, var_LLG - data.LL_RL);
        data.LL_RL = 0;
        savedata();
      }
    } else {
      data.statusRualoc = HIGH;
      if (data.LL_RL == 0) {
        data.LL_RL = var_LLG;
        savedata();
      }
    }
  } else {
    Blynk.virtualWrite(V17, data.statusRualoc);
  }
}
BLYNK_WRITE(V18)  // Time input
{
  if (keySet) {
    TimeInputParam t(param);
    if (t.hasStartTime()) {
      if (b == 0)
        data.b1_1_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 1)
        data.b2_1_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 2)
        data.b1_2_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 3)
        data.b2_2_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 4)
        data.b1_3_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 5)
        data.b2_3_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 6)
        data.b1_4_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 7)
        data.b2_4_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 8)
        data.bom_chanle_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      else if (b == 9)
        data.time_run_nenkhi = t.getStartHour() * 3600 + t.getStartMinute() * 60;
    }
    if (t.hasStopTime()) {
      if (b == 0)
        data.b1_1_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 1)
        data.b2_1_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 2)
        data.b1_2_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 3)
        data.b2_2_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 4)
        data.b1_3_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 5)
        data.b2_3_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 6)
        data.b1_4_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 7)
        data.b2_4_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 8)
        data.bom_chanle_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      else if (b == 9)
        data.time_stop_nenkhi = t.getStopHour() * 3600 + t.getStopMinute() * 60;
    }
  } else
    Blynk.virtualWrite(V18, 0);
}
BLYNK_WRITE(V28)  // hidden_all/visible_all
{
  if (param.asInt() == 0)
    hidden_all();
  else
    visible_all();
}
BLYNK_WRITE(V29)  // Info
{
  if (param.asInt() == 1) {
    terminal.clear();
    if (data.mode_cap2 == 0) {
      Blynk.virtualWrite(V10, "Chế độ bơm: Vận hành THỦ CÔNG");
    } else if ((data.mode_cap2 == 1) || (data.mode_cap2 == 2)) {
      int hour_start = data.bom_chanle_start / 3600;
      int minute_start = (data.bom_chanle_start - (hour_start * 3600)) / 60;
      int hour_stop = data.bom_chanle_stop / 3600;
      int minute_stop = (data.bom_chanle_stop - (hour_stop * 3600)) / 60;
      if (data.mode_cap2 == 1) {
        if (data.cap2_chanle == 0)
          Blynk.virtualWrite(V10, "Chế độ bơm: Bơm 1 tự động\nTắt máy vào: NGÀY CHẴN\nThời gian: ", hour_start, ":", minute_start, " - ", hour_stop, ":", minute_stop);
        else if (data.cap2_chanle == 1)
          Blynk.virtualWrite(V10, "Chế độ bơm: Bơm 1 tự động\nTắt máy vào: NGÀY LẺ\nThời gian: ", hour_start, ":", minute_start, " - ", hour_stop, ":", minute_stop);
      } else if (data.mode_cap2 == 2) {
        if (data.cap2_chanle == 0)
          Blynk.virtualWrite(V10, "Chế độ bơm: Bơm 2 tự động\nTắt máy vào: NGÀY CHẴN\nThời gian: ", hour_start, ":", minute_start, " - ", hour_stop, ":", minute_stop);
        else if (data.cap2_chanle == 1)
          Blynk.virtualWrite(V10, "Chế độ bơm: Bơm 2 tự động\nTắt máy vào: NGÀY LẺ\nThời gian: ", hour_start, ":", minute_start, " - ", hour_stop, ":", minute_stop);
      }
    } else if (data.mode_cap2 == 3) {
      int hour_start_b1_1 = data.b1_1_start / 3600;
      int minute_start_b1_1 = (data.b1_1_start - (hour_start_b1_1 * 3600)) / 60;
      int hour_stop_b1_1 = data.b1_1_stop / 3600;
      int minute_stop_b1_1 = (data.b1_1_stop - (hour_stop_b1_1 * 3600)) / 60;

      int hour_start_b2_1 = data.b2_1_start / 3600;
      int minute_start_b2_1 = (data.b2_1_start - (hour_start_b2_1 * 3600)) / 60;
      int hour_stop_b2_1 = data.b2_1_stop / 3600;
      int minute_stop_b2_1 = (data.b2_1_stop - (hour_stop_b2_1 * 3600)) / 60;

      int hour_start_b1_2 = data.b1_2_start / 3600;
      int minute_start_b1_2 = (data.b1_2_start - (hour_start_b1_2 * 3600)) / 60;
      int hour_stop_b1_2 = data.b1_2_stop / 3600;
      int minute_stop_b1_2 = (data.b1_2_stop - (hour_stop_b1_2 * 3600)) / 60;

      int hour_start_b2_2 = data.b2_2_start / 3600;
      int minute_start_b2_2 = (data.b2_2_start - (hour_start_b2_2 * 3600)) / 60;
      int hour_stop_b2_2 = data.b2_2_stop / 3600;
      int minute_stop_b2_2 = (data.b2_2_stop - (hour_stop_b2_2 * 3600)) / 60;

      int hour_start_b1_3 = data.b1_3_start / 3600;
      int minute_start_b1_3 = (data.b1_3_start - (hour_start_b1_3 * 3600)) / 60;
      int hour_stop_b1_3 = data.b1_3_stop / 3600;
      int minute_stop_b1_3 = (data.b1_3_stop - (hour_stop_b1_3 * 3600)) / 60;

      int hour_start_b2_3 = data.b2_3_start / 3600;
      int minute_start_b2_3 = (data.b2_3_start - (hour_start_b2_3 * 3600)) / 60;
      int hour_stop_b2_3 = data.b2_3_stop / 3600;
      int minute_stop_b2_3 = (data.b2_3_stop - (hour_stop_b2_3 * 3600)) / 60;

      int hour_start_b1_4 = data.b1_4_start / 3600;
      int minute_start_b1_4 = (data.b1_4_start - (hour_start_b1_4 * 3600)) / 60;
      int hour_stop_b1_4 = data.b1_4_stop / 3600;
      int minute_stop_b1_4 = (data.b1_4_stop - (hour_stop_b1_4 * 3600)) / 60;

      int hour_start_b2_4 = data.b2_4_start / 3600;
      int minute_start_b2_4 = (data.b2_4_start - (hour_start_b2_4 * 3600)) / 60;
      int hour_stop_b2_4 = data.b2_4_stop / 3600;
      int minute_stop_b2_4 = (data.b2_4_stop - (hour_stop_b2_4 * 3600)) / 60;

      Blynk.virtualWrite(V10, "Mode: Auto\nPump 1: ", hour_start_b1_1, "h", minute_start_b1_1, " -> ", hour_stop_b1_1, "h", minute_stop_b1_1, "\nPump 2: ", hour_start_b2_1, "h", minute_start_b2_1, " -> ", hour_stop_b2_1, "h", minute_stop_b2_1, "\nPump 1: ", hour_start_b1_2, "h", minute_start_b1_2, " -> ", hour_stop_b1_2, "h", minute_stop_b1_2, "\nPump 2: ", hour_start_b2_2, "h", minute_start_b2_2, " -> ", hour_stop_b2_2, "h", minute_stop_b2_2);
      Blynk.virtualWrite(V10, "\nPump 1: ", hour_start_b1_3, "h", minute_start_b1_3, " -> ", hour_stop_b1_3, "h", minute_stop_b1_3, "\nPump 2: ", hour_start_b2_3, "h", minute_start_b2_3, " -> ", hour_stop_b2_3, "h", minute_stop_b2_3, "\nPump 1: ", hour_start_b1_4, "h", minute_start_b1_4, " -> ", hour_stop_b1_4, "h", minute_stop_b1_4, "\nPump 2: ", hour_start_b2_4, "h", minute_start_b2_4, " -> ", hour_stop_b2_4, "h", minute_stop_b2_4);
    }
  } else terminal.clear();
  timer.restartTimer(timer_1);
  timer.restartTimer(timer_2);
}
//-------------------------------------------------------------------
BLYNK_WRITE(V31) {
  String dataS = param.asStr();
  if ((dataS == "ok") || (dataS == "Ok") || (dataS == "OK") || (dataS == "oK")) {
    if (clo_cache > 0) {
      data.clo = clo_cache;
      clo_cache = 0;
      data.time_clo = timestamp;
      Blynk.virtualWrite(V36, data.clo);
      savedata();
      terminal_luuluong.clear();
      Blynk.virtualWrite(V31, "Đã lưu - CLO:", data.clo, "kg");
    }
  }
}
BLYNK_WRITE(V32) {
  if (param.asFloat() > 0) {
    if (menu_gieng_luuluong == 1) {
      terminal_luuluong.clear();
      clo_cache = param.asFloat();
      Blynk.virtualWrite(V31, " Lượng CLO châm hôm nay:", clo_cache, "kg\n Vui lòng kiểm tra kỹ, nếu đúng hãy nhập 'OK' để lưu");
    }
  }
}
BLYNK_WRITE(V33) {
  if (param.asInt() == 1) {
    DateTime dt(data.time_clo);
    terminal_luuluong.clear();
    Blynk.virtualWrite(V31, "Châm CLO: ", data.clo, " kg vào lúc ", dt.hour(), ":", dt.minute(), "-", dt.day(), "/", dt.month(), "/", dt.year());
  }
}
BLYNK_WRITE(V34) {
  switch (param.asInt()) {
    case 0:
      {  //.....
        menu_gieng_luuluong = 0;
        terminal_luuluong.clear();
        break;
      }
    case 1:
      {  //Gieng
        menu_gieng_luuluong = 1;
        break;
      }
    case 2:
      {  //Clo
        menu_gieng_luuluong = 2;
        break;
      }
  }
  clo_cache = 0;
  Blynk.virtualWrite(V32, 0);
}
BLYNK_WRITE(V37) {
  var_LLG = param.asInt();
}
//-------------------------------------------------------------------
void connectionstatus() {
  if ((WiFi.status() != WL_CONNECTED)) {
    //WiFi.disconnect();
    //WiFi.mode(WIFI_STA);
    //WiFi.begin(ssid, password);
    //Blynk.config(auth);
    Serial.println("Khong ket noi WIFI");
  }
  if ((WiFi.status() == WL_CONNECTED) && (!Blynk.connected())) {
    data.reboot_num = data.reboot_num + 1;
    savedata();
    //Serial.print("data.reboot_num: ");
    //Serial.println(data.reboot_num);
    if (data.reboot_num == 1) {
      //Serial.println("Restart...");
      //Serial.print("data.reboot_num: ");
      //Serial.println(data.reboot_num);
      delay(1000);
      ESP.restart();
    }
    if (data.reboot_num % 5 == 0) {
      //Serial.print("data.reboot_num: ");
      //Serial.println(data.reboot_num);
      delay(1000);
      ESP.restart();
    }
  }
  if (Blynk.connected()) {
    if (data.reboot_num != 0) {
      data.reboot_num = 0;
      savedata();
    }
    //Serial.print("data.reboot_num: ");
    //Serial.println(data.reboot_num);
    //Serial.println("Blynk OK");
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
//-------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(5000);
  //---------------------------------------------------------------------------------
  pinMode(S0pin, OUTPUT);
  pinMode(S1pin, OUTPUT);
  pinMode(S2pin, OUTPUT);
  pinMode(S3pin, OUTPUT);

  emon0.current(A0, 102);  //Giếng
  emon1.current(A0, 96);   //Bơm 1
  emon2.current(A0, 86);   //Bơm 2
  emon3.current(A0, 81);   //Nén khí
  emon4.current(A0, 85);   //Van điện

  //Wire.begin();
  sensors.begin();  // DS18B20 start

  mcp.begin_I2C();

  mcp.pinMode(2, OUTPUT);
  mcp.digitalWrite(2, HIGH);
  mcp.pinMode(0, OUTPUT);
  mcp.digitalWrite(0, HIGH);
  mcp.pinMode(9, OUTPUT);
  mcp.digitalWrite(pinbom2, HIGH);  // Bom 2
  mcp.pinMode(11, OUTPUT);
  mcp.digitalWrite(pinvandien, HIGH);  // Rua loc
  mcp.pinMode(13, OUTPUT);
  mcp.digitalWrite(pinbom1, HIGH);  // Bom 1
  mcp.pinMode(14, OUTPUT);
  mcp.digitalWrite(pincap1, HIGH);  // Gieng

  rtc_module.begin();

  eeprom.initialize();
  eeprom.readBytes(address, sizeof(dataDefault), (byte*)&data);

  terminal.clear();

  timer.setTimeout(5000L, []() {
    timer_2 = timer.setInterval(243L, []() {
      readPressure();
      MeasureCmForSmoothing();
    });
    timer_1 = timer.setInterval(1103L, []() {
      readPower();
      readPower1();
      readPower2();
      readPower3();
      readPower4();
      //temperature();
      up();
      timer.restartTimer(timer_1);
      timer.restartTimer(timer_2);
    });
    timer_5 = timer.setInterval(15006L, []() {
      rtctime();
      timer.restartTimer(timer_1);
      timer.restartTimer(timer_2);
    });
    timer.setInterval(900005L, []() {
      connectionstatus();
      syncstatus();
      timer.restartTimer(timer_1);
      timer.restartTimer(timer_2);
    });
    timer.setInterval(long(((data.time_run_nenkhi + data.time_stop_nenkhi) * 1000) + 500), rualoc);
  });
}

void loop() {
  Blynk.run();
  timer.run();
  timer1.run();
}
