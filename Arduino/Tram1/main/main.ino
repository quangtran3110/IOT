/*-Color = 48c9b0
 *V0 - Btn Cap 1
 *V1 - Btn Cap 2
 *V2 - I0 - Cap 1
 *V3 - I1 - Cap 2
 *V4 - Ap Luc
 *V5 - Con lai
 *V6 - The tich
 *V7 - Che do Cap 2
 *V8 - Chon may bao ve
 *V9 - min A
 *V10 - max A
 *V11 - String
 *V12 - Chon thoi gian chay bom
 *V13 - timeinput
 *V14 - bao ve
 *V15 - thong bao
 *V16 - Rửa lọc
 *V17 - info
 *V18 - Btn Nen Khi
 *V19 - I2 - Nen Khi
 *V20 - date/time
 *V21 - terminal Clo
 *V22 - Do sau
 *V23 - Dung tich
 *V24 - LLG1_1m3
 *V25 - LLG1_24h
 *V26 - LLG1_RL
 *V27 - Status_VLG1
 *V28 - CLO input
 *V29 - Clo
 *V30 - check clo
*/

#define APP_DEBUG
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL83MCPItm"
#define BLYNK_TEMPLATE_NAME "Trạm Số 1"
#define BLYNK_AUTH_TOKEN "NkRHGVvq7kogEc7VhRgbcJXDJbFA-dMa"
#define BLYNK_FIRMWARE_VERSION "240101"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
const char* ssid = "Tram Bom So 1";
const char* password = "0943950555";
//-----------------------------
#include <Adafruit_MCP23X17.h>
Adafruit_MCP23X17 mcp;

const int pincap1 = 15;
const int pinbom = 14;
const int pinnenkhi = 12;
//-----------------------------
#include "EmonLib.h"
EnergyMonitor emon0, emon1, emon2;
//-----------------------------
#include <WidgetRTC.h>
#include "RTClib.h"
RTC_DS3231 rtc_module;
char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
char tz[] = "Asia/Ho_Chi_Minh";
//-----------------------------
#include <Eeprom24C32_64.h>
#define EEPROM_ADDRESS 0x57
static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);
const word address = 0;
//-----------------------------
#define filterSamples 121
int dai = 566;
int rong = 297;
int dosau = 130;
long distance, distance1, t;
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
//-----------------------------
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_name = "http://sgp1.blynk.cloud/external/api/";
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/IOT/main/Arduino/Tram1/main/build/esp8266.esp8266.nodemcuv2/main.ino.bin"
//-----------------------------
const int S0pin = 14;
const int S1pin = 12;
const int S2pin = 13;
const int S3pin = 15;
const int EN = 0;

bool key = false, keyp = true, keytank = true;
bool trip0 = false, trip1 = false, trip2 = false, trip_mcp = false;
bool key_memory = true, timer_I_status;
bool key_bom = true, key_gieng = true;
bool blynk_first_connect = false;
int LLG1_1m3;
int reboot_num = 0;
int c, b, i = 0;
int xSetAmpe = 0, xSetAmpe1 = 0, xSetAmpe2 = 0;
int btnState = HIGH, btnState1 = HIGH;
int timer_I;
float Irms0, Irms1, Irms2, value, Result1, clo_cache = 0;
unsigned long int xIrms0 = 0, xIrms1 = 0, xIrms2 = 0;
unsigned long int yIrms0 = 0, yIrms1 = 0, yIrms2 = 0;
unsigned long rest_time = 0, dem_bom = 0, dem_cap1 = 0;
uint32_t timestamp;

struct Data {
  byte SetAmpemax, SetAmpemin;
  byte SetAmpe1max, SetAmpe1min;
  byte SetAmpe2max, SetAmpe2min;
  byte mode_cap2, mode_run;
  int bom_chanle_start, bom_chanle_stop;
  int bom_moingay_start, bom_moingay_stop;
  byte status_g1, status_b1, status_nk1;
  int save_num;
  byte key_noti, rualoc;
  float clo;
  int time_clo, LLG1_RL;
} data, dataCheck;
const struct Data dataDefault = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

WidgetTerminal terminal(V11);
WidgetTerminal terminal_clo(V21);
WidgetRTC rtc_widget;
BlynkTimer timer, timer1;
BLYNK_CONNECTED() {
  rtc_widget.begin();
  blynk_first_connect = true;
}
//-------------------------------------------------------------------
void up() {
  String server_path = server_name + "batch/update?token=" + BLYNK_AUTH_TOKEN
                       + "&V2=" + Irms0
                       + "&V3=" + Irms1
                       + "&V4=" + float(Result1)
                       + "&V5=" + smoothDistance
                       + "&V6=" + volume1
                       + "&V19=" + Irms2;
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
    Blynk.setProperty(V11, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
  }
}
void on_cap1() {
  data.status_g1 = HIGH;
  mcp.digitalWrite(pincap1, data.status_g1);
}
void off_cap1() {
  data.status_g1 = LOW;
  mcp.digitalWrite(pincap1, data.status_g1);
}
void on_bom() {
  data.status_b1 = HIGH;
  mcp.digitalWrite(pinbom, data.status_b1);
}
void off_bom() {
  data.status_b1 = LOW;
  mcp.digitalWrite(pinbom, data.status_b1);
}
void on_nenkhi() {
  data.status_nk1 = HIGH;
  mcp.digitalWrite(pinnenkhi, data.status_nk1);
}
void off_nenkhi() {
  data.status_nk1 = LOW;
  mcp.digitalWrite(pinnenkhi, data.status_nk1);
}
void hidden() {
  Blynk.setProperty(V12, V13, V15, V14, V10, V9, V8, "isHidden", true);
}
void visible() {
  Blynk.setProperty(V12, V13, V15, V14, V10, V9, V8, "isHidden", false);
}
void off_time() {
  rest_time = rest_time + 1;
  if (Irms1 != 0) {
    off_bom();
  }
  if (Irms0 != 0) {
    off_cap1();
  }
}
void on_time() {
  rest_time = 0;
  if ((!trip1) && (Irms1 == 0)) {
    on_bom();
  }
  if ((!trip0) && (Irms0 == 0)) {
    on_cap1();
  }
}
//-------------------------------------------------------------------
void readPower()  // C0 - Cấp 1  - I0
{
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  float rms0 = emon0.calcIrms(1480);
  digitalWrite(EN, HIGH);
  if (rms0 < 2) {
    Irms0 = 0;
    yIrms0 = 0;
    if (rest_time == 0) {
      if (dem_cap1 == 0) {
        dem_cap1 = millis();
      }
      if ((unsigned long)(millis() - dem_cap1) > 900000) {  //15p
        dem_cap1 = 0;
        if (data.key_noti) {
          //Blynk.logEvent("info", String("Bơm Giếng không chạy. Xin kiểm tra."));
        }
      }
    }
  } else if (rms0 >= 2) {
    yIrms0 = yIrms0 + 1;
    Irms0 = rms0;
    if (yIrms0 > 3) {
      dem_cap1 = 0;
      if ((rest_time > 2) && (data.key_noti) && (key_gieng)) {
        key_gieng = false;
        Blynk.logEvent("info", String("Bơm Giếng không tắt. Xin kiểm tra."));
        timer1.setInterval(900000L, []() {  //15p
          key_gieng = true;
        });
      }
      if ((Irms0 >= data.SetAmpemax) || (Irms0 <= data.SetAmpemin)) {
        xSetAmpe = xSetAmpe + 1;
        if ((xSetAmpe >= 2) && (keyp)) {
          off_cap1();
          xSetAmpe = 0;
          trip0 = true;
          if (data.key_noti) {
            Blynk.logEvent("error", String("Cấp 1 lỗi: ") + Irms0 + String(" A"));
          }
        }
      } else {
        xSetAmpe = 0;
      }
    }
  }
  //Blynk.virtualWrite(V2, Irms0);
}
void readPower1()  // C1 - Bơm    - I1
{
  Blynk.run();
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  float rms1 = emon1.calcIrms(1480);
  digitalWrite(EN, HIGH);
  if (rms1 < 2) {
    Irms1 = 0;
    yIrms1 = 0;
    if (rest_time == 0) {
      if (dem_bom == 0) {
        dem_bom = millis();
      }
      if ((unsigned long)(millis() - dem_bom) > 1800000) {  //30p
        dem_bom = 0;
        if (data.key_noti) {
          //Blynk.logEvent("info", String("Bơm cấp 2 không chạy. Xin kiểm tra."));
        }
      }
    }
  } else if (rms1 >= 2) {
    Irms1 = rms1;
    yIrms1 = yIrms1 + 1;
    if (yIrms1 > 3) {
      dem_bom = 0;
      if ((rest_time > 2) && (data.key_noti) && (key_bom)) {
        key_bom = false;
        Blynk.logEvent("info", String("Bơm cấp 2 không tắt. Xin kiểm tra."));
        timer1.setInterval(900000L, []() {  //15p
          key_bom = true;
        });
      }
      if ((Irms1 >= data.SetAmpe1max) || (Irms1 <= data.SetAmpe1min)) {
        xSetAmpe1 = xSetAmpe1 + 1;
        if ((xSetAmpe1 >= 2) && (keyp)) {
          off_bom();
          xSetAmpe1 = 0;
          trip1 = true;
          if (data.key_noti) {
            Blynk.logEvent("error", String("Cấp 2 lỗi: ") + Irms1 + String(" A"));
          }
        }
      } else {
        xSetAmpe1 = 0;
      }
    }
  }
  //Blynk.virtualWrite(V3, Irms1);
}
void readPower2()  // C2 - Nen khi- I2
{
  Blynk.run();
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  float rms2 = emon2.calcIrms(1480);
  digitalWrite(EN, HIGH);
  if (rms2 < 2) {
    Irms2 = 0;
    yIrms2 = 0;
  } else if (rms2 >= 2) {
    Irms2 = rms2;
    yIrms2 = yIrms2 + 1;
    if ((yIrms2 > 3) && ((Irms2 >= data.SetAmpe2max) || (Irms2 <= data.SetAmpe2min))) {
      xSetAmpe2 = xSetAmpe2 + 1;
      if ((xSetAmpe2 >= 2) && (keyp)) {
        off_nenkhi();
        xSetAmpe2 = 0;
        trip2 = true;
        if (data.key_noti) {
          Blynk.logEvent("error", String("Máy nén khí lỗi: ") + Irms2 + String(" A"));
        }
      }
    } else {
      xSetAmpe2 = 0;
    }
  }
  //Blynk.virtualWrite(V19, Irms2);
}
//-------------------------------------------------------------------
void readPressure()  //C4 - Ap Luc
{
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  float sensorValue = analogRead(A0);
  digitalWrite(EN, HIGH);
  float Result;
  Result = (((sensorValue - 199) * 10) / (990 - 199));
  if (Result > 0) {
    value += Result;
    Result1 = value / 16;
    value -= Result1;
  }
}
void MeasureCmForSmoothing()  //Muc Nuoc
{
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  digitalWrite(EN, LOW);
  float sensorValue = analogRead(A0);
  digitalWrite(EN, HIGH);
  distance1 = (((sensorValue - 196.5) * 500) / (980 - 196.5));
  if (distance1 > 0) {
    smoothDistance = digitalSmooth(distance1, sensSmoothArray1);
    volume1 = (dai * smoothDistance * rong) / 1000000;
    if ((smoothDistance - dosau >= 20) && (data.key_noti) && (keytank)) {
      Blynk.logEvent("info", String("Nước trong bể cao vượt mức ") + (smoothDistance - dosau) + String(" cm"));
      keytank = false;
      timer1.setTimeout(1800000L, []() {
        keytank = true;
      });
    }
  }
}
//-------------------------------------------------------------------
void checkPhysicalButton() {
  if (mcp.digitalRead(6) == LOW) {  // Irms0
    if (btnState != LOW) {
      if (data.status_g1 == LOW) {
        off_cap1();
      } else {
        on_cap1();
        Blynk.virtualWrite(V0, !data.status_g1);
      }
    }
    btnState = LOW;
  }
  if (mcp.digitalRead(6) == HIGH) { btnState = HIGH; }

  if (mcp.digitalRead(7) == LOW) {  // Irms1
    if (btnState1 != LOW) {
      if (data.status_b1 == LOW) {
        off_bom();
      } else on_bom();
    }
    btnState1 = LOW;
  }
  if (mcp.digitalRead(7) == HIGH) { btnState1 = HIGH; }
}
void rtctime() {
  DateTime now = rtc_module.now();
  if (blynk_first_connect == true) {
    if ((now.day() != day()) || (now.hour() != hour()) || ((now.minute() - minute() > 2) || (minute() - now.minute() > 2))) {
      rtc_module.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
      DateTime now = rtc_module.now();
    }
  }
  Blynk.virtualWrite(V20, daysOfTheWeek[now.dayOfTheWeek()], ", ", now.day(), "/", now.month(), "/", now.year(), " - ", now.hour(), ":", now.minute(), ":", now.second());
  timestamp = now.unixtime();

  int nowtime = (now.hour() * 3600 + now.minute() * 60);

  if (data.mode_cap2 == 1) {   // Chạy Tu Dong
    if (data.mode_run == 0) {  //Ngay chan tat may
      if (now.day() % 2 == 0) {
        if (nowtime > data.bom_chanle_stop) {
          off_time();
        }
        if (nowtime < data.bom_chanle_stop) {
          on_time();
        }
      }
      if (now.day() % 2 != 0) {
        if ((nowtime > data.bom_chanle_start)) {
          on_time();
        }
      }
    }
    if (data.mode_run == 1) {  //Ngay le tat may
      if (now.day() % 2 != 0) {
        if (nowtime > data.bom_chanle_stop) {
          off_time();
        }
        if (nowtime < data.bom_chanle_stop) {
          on_time();
        }
      }
      if (now.day() % 2 == 0) {
        if (nowtime > data.bom_chanle_start) {
          on_time();
        }
      }
    }
    if (data.mode_run == 2) {  //Moi ngày
      if (data.bom_moingay_start > data.bom_moingay_stop) {
        if ((nowtime > data.bom_moingay_stop) && (nowtime < data.bom_moingay_start)) {
          off_time();
        }
        if ((nowtime < data.bom_moingay_stop) || (nowtime > data.bom_moingay_start)) {
          on_time();
        }
      }
      if (data.bom_moingay_start < data.bom_moingay_stop) {
        if ((nowtime > data.bom_moingay_stop) || (nowtime < data.bom_moingay_start)) {
          off_time();
        }
        if ((nowtime < data.bom_moingay_stop) && (nowtime > data.bom_moingay_start)) {
          on_time();
        }
      }
    }
  }
}
//-------------------------------------------------------------------
BLYNK_WRITE(V0)  // Gieng
{
  if ((key) && (!trip0)) {
    if (param.asInt() == LOW) {
      off_cap1();
    } else {
      on_cap1();
    }
  }
  Blynk.virtualWrite(V0, data.status_g1);
}
BLYNK_WRITE(V1)  // Bơm
{
  if ((key) && (!trip1)) {
    if (param.asInt() == LOW) {
      off_bom();
    } else {
      on_bom();
    }
  }
  Blynk.virtualWrite(V1, data.status_b1);
}
BLYNK_WRITE(V7)  // Chọn chế độ Cấp 2
{
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          data.mode_cap2 = 0;
          break;
        }
      case 1:
        {  // Auto
          data.mode_cap2 = 1;
          break;
        }
    }
  } else
    Blynk.virtualWrite(V7, data.mode_cap2);
}
BLYNK_WRITE(V8)  // Chon máy cài đặt bảo vệ
{
  switch (param.asInt()) {
    case 0:
      {  // Gieng
        c = 0;
        Blynk.virtualWrite(V9, 0);
        Blynk.virtualWrite(V10, 0);
        break;
      }
    case 1:
      {  // Gieng
        c = 1;
        Blynk.virtualWrite(V9, data.SetAmpemin);
        Blynk.virtualWrite(V10, data.SetAmpemax);
        break;
      }
    case 2:
      {  // Bom
        c = 2;
        Blynk.virtualWrite(V9, data.SetAmpe1min);
        Blynk.virtualWrite(V10, data.SetAmpe1max);
        break;
      }
    case 3:
      {  // Nén khí
        c = 3;
        Blynk.virtualWrite(V9, data.SetAmpe2min);
        Blynk.virtualWrite(V10, data.SetAmpe2max);
        break;
      }
  }
}
BLYNK_WRITE(V9)  // min
{
  if (key) {
    if (c == 1) {
      data.SetAmpemin = param.asInt();
    } else if (c == 2) {
      data.SetAmpe1min = param.asInt();
    } else if (c == 3) {
      data.SetAmpe2min = param.asInt();
    }
  } else {
    Blynk.virtualWrite(V9, 0);
  }
}
BLYNK_WRITE(V10)  // max
{
  if (key) {
    if (c == 1) {
      data.SetAmpemax = param.asInt();
    } else if (c == 2) {
      data.SetAmpe1max = param.asInt();
    } else if (c == 3) {
      data.SetAmpe2max = param.asInt();
    }
  } else {
    Blynk.virtualWrite(V10, 0);
  }
}
BLYNK_WRITE(V11)  // String
{
  String dataS = param.asStr();
  if (dataS == "ts1") {
    terminal.clear();
    key = true;
    Blynk.virtualWrite(V11, "Người vận hành: 'V.Tài'\nKích hoạt trong 15s\n");
    timer1.setTimeout(15000, []() {
      key = false;
      terminal.clear();
    });
  } else if (dataS == "active") {
    terminal.clear();
    key = true;
    visible();
    Blynk.virtualWrite(V11, "KHÔNG sử dụng phần mềm cho đến khi thông báo này mất.\n");
  } else if (dataS == "deactive") {
    terminal.clear();
    key = false;
    hidden();
    Blynk.virtualWrite(V11, "Ok!\nNhập mã để điều khiển!\n");
  } else if (dataS == "save") {
    terminal.clear();
    savedata();
    Blynk.virtualWrite(V11, "Đã lưu cài đặt.\n");
  } else if (dataS == "reset") {
    terminal.clear();
    trip2 = false;
    trip1 = false;
    trip0 = false;
    Blynk.virtualWrite(V11, "Đã RESET! \nNhập mã để điều khiển!\n");
  } else if (dataS == "update") {
    terminal.clear();
    Blynk.virtualWrite(V11, "UPDATE FIRMWARE...");
    update_fw();
  } else if (dataS == "rst") {
    terminal.clear();
    Blynk.virtualWrite(V11, "ESP khởi động lại sau 3s");
    delay(3000);
    ESP.restart();
  } else {
    Blynk.virtualWrite(V11, "Mật mã sai.\nVui lòng nhập lại!\n");
  }
}
BLYNK_WRITE(V12)  // Chọn thời gian chạy Bơm
{
  switch (param.asInt()) {
    case 0:
      {
        Blynk.virtualWrite(V13, 0, 0, tz);
        break;
      }
    case 1:
      {
        if (key) {
          data.mode_run = 0;  //Chẵn
        }
        Blynk.virtualWrite(V13, data.bom_chanle_start, data.bom_chanle_stop, tz);
        break;
      }
    case 2:
      {
        if (key)
          data.mode_run = 1;  //Lẻ
        Blynk.virtualWrite(V13, data.bom_chanle_start, data.bom_chanle_stop, tz);
        break;
      }
    case 3:
      {
        if (key)
          data.mode_run = 2;  //Mỗi ngày
        Blynk.virtualWrite(V13, data.bom_moingay_start, data.bom_moingay_stop, tz);
        break;
      }
  }
}
BLYNK_WRITE(V13)  // Time input
{
  if (key) {
    TimeInputParam t(param);
    if (t.hasStartTime()) {
      if ((data.mode_run == 0) || (data.mode_run == 1)) {
        data.bom_chanle_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      }
      if (data.mode_run == 2) {
        data.bom_moingay_start = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      }
    }
    if (t.hasStopTime()) {
      if ((data.mode_run == 0) || (data.mode_run == 1)) {
        data.bom_chanle_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      }
      if (data.mode_run == 2) {
        data.bom_moingay_stop = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      }
    }
  } else
    Blynk.virtualWrite(V13, 0);
}
BLYNK_WRITE(V14)  // Bảo vệ
{
  if (key) {
    int data13 = param.asInt();
    if (data13 == LOW) {
      keyp = false;
    } else {
      keyp = true;
    }
  } else
    Blynk.virtualWrite(V14, keyp);
}
BLYNK_WRITE(V15)  // Thông báo
{
  if (key) {
    if (param.asInt() == LOW) {
      data.key_noti = false;
      savedata();
    } else {
      data.key_noti = true;
      savedata();
    }
  } else
    Blynk.virtualWrite(V15, data.key_noti);
}
BLYNK_WRITE(V16)  // Rửa lọc
{
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  //Tắt
          data.rualoc = 0;
          if (data.LLG1_RL != 0) {
            Blynk.virtualWrite(V26, LLG1_1m3 - data.LLG1_RL);
            data.LLG1_RL = 0;
            savedata();
          }
          break;
        }
      case 1:
        {  //RL 1
          data.rualoc = 1;
          if (data.LLG1_RL == 0) {
            data.LLG1_RL = LLG1_1m3;
          }
          break;
        }
    }
    savedata();
  } else {
    Blynk.virtualWrite(V16, data.rualoc);
  }
}
BLYNK_WRITE(V17)  // Info
{
  if (param.asInt() == 1) {
    terminal.clear();
    if (data.mode_cap2 == 0) {
      Blynk.virtualWrite(V11, "Chế độ bơm: Vận hành THỦ CÔNG.");
    } else if (data.mode_cap2 == 1) {
      if (data.mode_run == 0) {  //Chẵn
        int chanle_start_h = data.bom_chanle_start / 3600;
        int chanle_start_m = (data.bom_chanle_start - (chanle_start_h * 3600)) / 60;
        int chanle_stop_h = data.bom_chanle_stop / 3600;
        int chanle_stop_m = (data.bom_chanle_stop - (chanle_stop_h * 3600)) / 60;
        Blynk.virtualWrite(V11, "Chế độ bơm: Tự động - Chẵn\nThời gian: ", chanle_stop_h, " : ", chanle_stop_m, " -> ", chanle_start_h, " : ", chanle_start_m);
      }
      if (data.mode_run == 1) {  //Lẻ
        int chanle_start_h = data.bom_chanle_start / 3600;
        int chanle_start_m = (data.bom_chanle_start - (chanle_start_h * 3600)) / 60;
        int chanle_stop_h = data.bom_chanle_stop / 3600;
        int chanle_stop_m = (data.bom_chanle_stop - (chanle_stop_h * 3600)) / 60;
        Blynk.virtualWrite(V11, "Chế độ bơm: Tự động - Lẻ\nThời gian: ", chanle_stop_h, " : ", chanle_stop_m, " -> ", chanle_start_h, " : ", chanle_start_m);
      }
      if (data.mode_run == 2) {  //Mỗi ngày
        int moingay_start_h = data.bom_moingay_start / 3600;
        int moingay_start_m = (data.bom_moingay_start - (moingay_start_h * 3600)) / 60;
        int moingay_stop_h = data.bom_moingay_stop / 3600;
        int moingay_stop_m = (data.bom_moingay_stop - (moingay_stop_h * 3600)) / 60;
        Blynk.virtualWrite(V11, "Chế độ bơm: Tự động - Mỗi ngày\nThời gian: ", moingay_stop_h, " : ", moingay_stop_m, " -> ", moingay_start_h, " : ", moingay_start_m);
      }
    }
  } else {
    terminal.clear();
  }
}
BLYNK_WRITE(V18)  // Nen Khi
{
  if ((key) && (!trip2)) {
    if (param.asInt() == LOW) {
      off_nenkhi();
    } else on_nenkhi();
  }
  Blynk.virtualWrite(V18, data.status_nk1);
}
//-------------------------
BLYNK_WRITE(V24)  // Lưu lượng G1_1m3
{
  LLG1_1m3 = param.asInt();
}
//-------------------------
BLYNK_WRITE(V21)  //String Clo
{
  String dataS = param.asStr();
  if ((dataS == "ok") || (dataS == "Ok") || (dataS == "OK") || (dataS == "oK")) {
    if (clo_cache > 0) {
      data.clo = clo_cache;
      clo_cache = 0;
      data.time_clo = timestamp;
      Blynk.virtualWrite(V29, data.clo);
      savedata();
      terminal_clo.clear();
      Blynk.virtualWrite(V21, "Đã lưu - CLO:", data.clo, "kg");
    }
  }
}
BLYNK_WRITE(V28)  //Clo input
{
  if (param.asFloat() > 0) {
    terminal_clo.clear();
    clo_cache = param.asFloat();
    Blynk.virtualWrite(V21, " Lượng CLO châm hôm nay:", clo_cache, "kg\n Vui lòng kiểm tra kỹ, nếu đúng hãy nhập 'OK' để lưu");
  }
}
BLYNK_WRITE(V30)  //Check Clo
{
  if (param.asInt() == 1) {
    DateTime dt(data.time_clo);
    terminal_clo.clear();
    Blynk.virtualWrite(V21, "Châm CLO:", data.clo, "kg vào lúc", dt.hour(), ":", dt.minute(), "-", dt.day(), "/", dt.month(), "/", dt.year());
  }
}
//----------------------------------------------------
void connectionstatus() {
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.println("Khong ket noi WIFI");
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
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(ssid, password);
    }
  }
  if (Blynk.connected()) {
    if (reboot_num != 0) {
      reboot_num = 0;
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
  pinMode(EN, OUTPUT);

  emon0.current(A0, 130);
  emon1.current(A0, 130);
  emon2.current(A0, 130);

  if (!mcp.begin_I2C()) {
    trip_mcp = true;
  }

  mcp.pinMode(6, INPUT);
  mcp.pinMode(7, INPUT);
  mcp.pinMode(pincap1, OUTPUT);
  mcp.digitalWrite(pincap1, HIGH);  // Bom 1
  mcp.pinMode(pinbom, OUTPUT);
  mcp.digitalWrite(pinbom, HIGH);  // Bom 2
  mcp.pinMode(pinnenkhi, OUTPUT);
  mcp.digitalWrite(pinnenkhi, HIGH);

  rtc_module.begin();
  eeprom.initialize();
  eeprom.readBytes(address, sizeof(dataDefault), (byte*)&data);
  //------------------------------------
  timer.setTimeout(5000L, []() {
    timer.setInterval(201L, []() {
      readPressure();
      MeasureCmForSmoothing();
    });
    timer_I = timer.setInterval(983L, []() {
      readPower();
      readPower1();
      readPower2();
      up();
      timer.restartTimer(timer_I);
    });
    timer.setInterval(15006L, []() {
      rtctime();
      timer.restartTimer(timer_I);
    });
    timer.setInterval(900005L, []() {
      connectionstatus();
      timer.restartTimer(timer_I);
    });
    terminal.clear();
  });
}
void loop() {
  Blynk.run();
  timer.run();
  timer1.run();
}
