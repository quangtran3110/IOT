/**DC7633
 *V0 - Button C2 - 1
 *V1 - Button C2 - 2
 *V2 - Button C1 - 1
 *V3 - Button C1 - 2
 *V4 - Chọn người vận hành
 *V5 - MENU motor
 *V6 - min
 *V7 - max
 *V8 - info
 *V9 - Ngày/Giờ
 *V10 - terminal key
 *V11 - Thời gian chạy Bơm
 *V12 - Hide/visible
 *V13 - Bảo vệ
 *V14 - Ap luc
 *V15 - 
 *V16 - 
 *V17 - Thông báo
 *V18 - time input
 *V19 -

 *V20 - I0 - Cấp 1 - 1
 *V21 - I1 - Cấp 1 - 2
 *V22 - I2 - Bơm 2
 *V23 - I3 - Bơm 2
 *V24 - Dung tich
 *V25 - Thể tích
 *V26 - Còn lại
 *V27 - Độ sâu
 *V28 -
 *V29 -
 *V30 -
 *V31 -
 *V32 -
 *V33 -
 *V34 -
 *V35 -
 *V36 -
 *V37 -
 *V40 -
 *V41 -
 *V42 -
 *V43 -
 *
 */

/*
#define BLYNK_TEMPLATE_ID "TMPLbPLEi8uh"
#define BLYNK_TEMPLATE_NAME "Trạm Mộc Hóa"
#define BLYNK_AUTH_TOKEN "tKNZ99XnCSeCsoDEva3kx-O0YWw83nMn"
*/
#define BLYNK_TEMPLATE_ID "TMPL6coHtFMJ-"
#define BLYNK_TEMPLATE_NAME "TRẠM 3 BPT"
#define BLYNK_AUTH_TOKEN "Xd_XI0fm9nIsXBvvMZ6pjEtRd0irLLR2"

#define BLYNK_FIRMWARE_VERSION "240503"
#define BLYNK_PRINT Serial
const char* ssid = "KwacoBlynk";
const char* password = "Password";
//const char* ssid = "Wifi";
//const char* password = "Password";

#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
//--------------
#include "PCF8575.h"
PCF8575 pcf8575(0x20);
//--------------
#include "EmonLib.h"
EnergyMonitor emon0, emon1, emon2, emon3;
//--------------
#include <WidgetRTC.h>
#include <TimeLib.h>
#include "RTClib.h"
RTC_DS3231 rtc_module;
//--------------
#include <Wire.h>
#include <OneWire.h>
OneWire oneWire(D3);
//--------------
#include <Eeprom24C32_64.h>
#define EEPROM_ADDRESS 0x57
static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);
//--------------
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/work/kwaco/mochoa/main.ino.bin"
String server_name = "http://sgp1.blynk.cloud/external/api/";
//--------------
#define filterSamples 121
const int S0pin = 14;
const int S1pin = 12;
const int S2pin = 13;
const int S3pin = 15;
const word address = 0;

const int dai = 750;
const int rong = 750;
const int dosau = 330;

const int btn_left = P7;
const int btn_right = P6;
const int btn_mid = P5;
const int btn_g1 = P4;
const int btn_g2 = P3;
const int pin_usb = P0;
const int pin4067 = P15;

const bool b1 = HIGH;
const bool b2 = LOW;
long t;
long m = 60000;

char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
char tz[] = "Asia/Ho_Chi_Minh";
bool key = false, keyp = true, keynoti = true;
bool trip0 = false, trip1 = false, trip2 = false, trip3 = false;
bool temp_0 = true, temp_1 = true;
bool noti_1 = true, noti_2 = true;
bool blynk_first_connect = false;
int timer_I, timer_run_main;
int i = 0;  //Biến đếm áp lực
int j = 0;  //Biến đếm cấp 1 không chạy
int c, b, check_connect = 0;
int status_g1 = HIGH, status_g2 = HIGH;
int timer_1, timer_2;
int xSetAmpe = 0, xSetAmpe1 = 0, xSetAmpe2 = 0, xSetAmpe3 = 0;
float Irms0, Irms1, Irms2, Irms3, value, Result1;
unsigned long int xIrms0 = 0, xIrms1 = 0, xIrms2 = 0, xIrms3 = 0;
unsigned long int yIrms0 = 0, yIrms1 = 0, yIrms2 = 0, yIrms3 = 0;
unsigned long timerun;

int volume, volume1, smoothDistance;
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

struct Data {
  byte SetAmpemax, SetAmpemin;
  byte SetAmpe1max, SetAmpe1min;
  byte SetAmpe2max, SetAmpe2min;
  byte SetAmpe3max, SetAmpe3min;
  byte mode_cap2;
  byte reboot_num;
  int start_time, stop_time;
  int save_num;
  byte tank_min;
  float pre_min;
  byte check_changeday;
  byte status_btn_mid;
  byte status_btn_left;
  byte status_direct;
} data, dataCheck;
const struct Data dataDefault = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

WidgetTerminal terminal(V10);
WidgetRTC rtc;
BlynkTimer timer, timeout;
BLYNK_CONNECTED() {
  rtc.begin();
  blynk_first_connect = true;
}
//-------------------------------------------------------------------
void savedata() {
  if (memcmp(&data, &dataCheck, sizeof(dataDefault)) == 0) {
    // Serial.println("structures same no need to write to EEPROM");
  } else {
    // Serial.println("\nWrite bytes to EEPROM memory...");
    data.save_num = data.save_num + 1;
    eeprom.writeBytes(address, sizeof(dataDefault), (byte*)&data);
  }
}
void reset_4067() {
  pcf8575.digitalWrite(pin4067, HIGH);
  delay(3000);
  pcf8575.digitalWrite(pin4067, LOW);
}
//-----------------------------
void on_G1() {  //Dùng thường đóng
  status_g1 = HIGH;
  pcf8575.digitalWrite(btn_g1, status_g1);
}
void off_G1() {
  status_g1 = LOW;
  pcf8575.digitalWrite(btn_g1, status_g1);
}
void on_G2() {  //Dùng thường đóng
  status_g2 = HIGH;
  pcf8575.digitalWrite(btn_g2, status_g2);
}
void off_G2() {
  status_g2 = LOW;
  pcf8575.digitalWrite(btn_g2, status_g2);
}
void run_main() {
  long time_start;
  if (timerun >= 60000) {
    time_start = 1000;
  } else time_start = 24000;
  data.status_btn_left = HIGH;
  pcf8575.digitalWrite(btn_left, data.status_btn_left);
  pcf8575.digitalWrite(btn_mid, data.status_btn_mid);
  timer_run_main = timeout.setTimeout(time_start, []() {
    pcf8575.digitalWrite(btn_left, !data.status_btn_left);
  });
  savedata();
}
void off_main() {
  timeout.deleteTimer(timer_run_main);
  data.status_btn_left = LOW;
  pcf8575.digitalWrite(btn_left, !data.status_btn_left);
  savedata();
}
void on_direct() {  //Dùng thường hở
  data.status_direct = HIGH;
  pcf8575.digitalWrite(btn_right, !data.status_direct);
  savedata();
}
void off_direct() {
  data.status_direct = LOW;
  pcf8575.digitalWrite(btn_right, !data.status_direct);
  savedata();
}
//-----------------------------
void hidden() {
  Blynk.setProperty(V11, "isHidden", true);
  Blynk.setProperty(V18, "isHidden", true);
  Blynk.setProperty(V7, "isHidden", true);
  Blynk.setProperty(V6, "isHidden", true);
  Blynk.setProperty(V5, "isHidden", true);
  Blynk.setProperty(V17, "isHidden", true);
  Blynk.setProperty(V13, "isHidden", true);
}
void visible() {
  Blynk.setProperty(V11, "isHidden", false);
  Blynk.setProperty(V18, "isHidden", false);
  Blynk.setProperty(V7, "isHidden", false);
  Blynk.setProperty(V6, "isHidden", false);
  Blynk.setProperty(V5, "isHidden", false);
  Blynk.setProperty(V17, "isHidden", false);
  Blynk.setProperty(V13, "isHidden", false);
}
void syncstatus() {
  Blynk.virtualWrite(V4, data.mode_cap2);
  Blynk.virtualWrite(V2, status_g1);
  Blynk.virtualWrite(V3, status_g2);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
BLYNK_WRITE(V20) {  // Cấp 1 - 1 - I0
  float rms0 = param.asFloat();
  if (rms0 < 3) {
    Irms0 = 0;
    yIrms0 = 0;
  } else if (rms0 >= 3) {
    Irms0 = rms0;
    yIrms0 = yIrms0 + 1;
    if ((yIrms0 > 3) && ((Irms0 >= data.SetAmpemax) || (Irms0 <= data.SetAmpemin))) {
      xSetAmpe = xSetAmpe + 1;
      if ((xSetAmpe >= 4) && (keyp)) {
        off_G1();
        xSetAmpe = 0;
        trip0 = true;
        if (keynoti) {
          Blynk.logEvent("error", String("Giếng 1 lỗi: ") + Irms0 + String(" A"));
        }
      }
    } else {
      xSetAmpe = 0;
    }
  }
}
void readPower1()  // C3 - Cấp 1 - 2 - I1
{
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float rms1 = emon1.calcIrms(740);
  if (rms1 < 3) {
    Irms1 = 0;
    yIrms1 = 0;
  } else if (rms1 >= 3) {
    Irms1 = rms1;
    yIrms1 = yIrms1 + 1;
    if ((yIrms1 > 3) && ((Irms1 >= data.SetAmpe1max) || (Irms1 <= data.SetAmpe1min))) {
      xSetAmpe1 = xSetAmpe1 + 1;
      if ((xSetAmpe1 >= 3) && (keyp)) {
        off_G2();
        trip1 = true;
        xSetAmpe1 = 0;
        if (keynoti) {
          Blynk.logEvent("error", String("Giếng 2 lỗi: ") + Irms1 + String(" A"));
        }
      }
    } else {
      xSetAmpe1 = 0;
    }
  }
  //Blynk.virtualWrite(V21, Irms1);
}
void readPower2()  // C4 - Bơm 1  - I2
{
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  float rms2 = emon2.calcIrms(740);
  if (rms2 < 3) {
    Irms2 = 0;
    yIrms2 = 0;
  } else if (rms2 >= 3) {
    Irms2 = rms2;
    yIrms2 = yIrms2 + 1;
    if ((yIrms2 > 15) && ((Irms2 >= data.SetAmpe2max) || ((Irms2 <= data.SetAmpe2min) && (Result1 < 2.4)))) {
      xSetAmpe2 = xSetAmpe2 + 1;
      if ((xSetAmpe2 >= 2) && (keyp)) {
        off_main();
        xSetAmpe2 = 0;
        trip2 = true;
        if (keynoti) {
          Blynk.logEvent("error", String("Bơm 1 lỗi: ") + Irms2 + String(" A"));
        }
      }
    } else {
      xSetAmpe2 = 0;
    }
  }
  //Blynk.virtualWrite(V22, Irms2);
}
void readPower3()  // C5 - Bơm 2  - I3
{
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  float rms3 = emon3.calcIrms(740);
  if (rms3 < 3) {
    Irms3 = 0;
    yIrms3 = 0;
  } else if (rms3 >= 3) {
    Irms3 = rms3;
    yIrms3 = yIrms3 + 1;
    if ((yIrms3 > 3) && ((Irms3 >= data.SetAmpe3max) || ((Irms3 <= data.SetAmpe3min) && (Result1 < 2.4)))) {
      xSetAmpe3 = xSetAmpe3 + 1;
      if ((xSetAmpe3 >= 15) && (keyp)) {
        off_main();
        xSetAmpe3 = 0;
        trip3 = true;
        if (keynoti) {
          Blynk.logEvent("error", String("Bơm 2 lỗi: ") + Irms3 + String(" A"));
        }
      }
    } else {
      xSetAmpe3 = 0;
    }
  }
  //Blynk.virtualWrite(V23, Irms3);
}
//-------------------------------------------------------------------
void readPressure()  // C1 - Ap Luc
{
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float sensorValue = analogRead(A0);
  float Result;
  Result = ((sensorValue - 193) * 10) / (925 - 193);
  if (Result > 0) {
    value += Result;
    Result1 = value / 16.0;
    value -= Result1;
  }
}
void MeasureCmForSmoothing() {
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float sensorValue = analogRead(A0);
  //Serial.print("Nuoc: ");
  //Serial.println(sensorValue);
  distance1 = (((sensorValue - 190) * 800) / (890 - 190));
  //Serial.print("Do sau: ");
  //Serial.println(distance1);
  if (distance1 > 0) {
    smoothDistance = digitalSmooth(distance1, sensSmoothArray1);
    volume1 = (dai * smoothDistance * rong) / 1000000;
  }
  //Serial.println(sensorValue);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void rtctime() {
  timerun = millis();
  DateTime now = rtc_module.now();
  if (blynk_first_connect == true) {
    if ((now.day() != day()) || (now.hour() != hour()) || ((now.minute() - minute() > 2) || (minute() - now.minute() > 2))) {
      rtc_module.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
      DateTime now = rtc_module.now();
    }
  }
  Blynk.virtualWrite(V9, daysOfTheWeek[now.dayOfTheWeek()], ", ", now.day(), "/", now.month(), "/", now.year(), " - ", now.hour(), ":", now.minute(), ":", now.second());

  int nowtime = (now.hour() * 3600 + now.minute() * 60);

  if (data.mode_cap2 == 2) {                 //Auto
    if (data.start_time < data.stop_time) {  //Nếu thời gian nghỉ lớn hơn thời gian chạy
      if ((nowtime > data.stop_time) || (nowtime < data.start_time)) {
        if (data.check_changeday == 0) {
          data.check_changeday = 1;
          data.status_btn_mid = !data.status_btn_mid;
          timerun = 0;
          if (data.status_direct == HIGH) off_direct();
          run_main();
        } else {
          if (data.status_direct == HIGH) off_direct();
          if (data.status_btn_mid == b1) {
            if ((Irms2 == 0) && (!trip2)) run_main();
          } else if (data.status_btn_mid == b2) {
            if ((Irms3 == 0) && (!trip3)) run_main();
          }
        }
      }
      if ((nowtime > data.start_time) && (nowtime < data.stop_time)) {
        if (data.check_changeday == 1) {
          data.check_changeday = 0;
          savedata();
        }
        if (data.status_btn_mid == b1) {
          if ((Irms2 == 0) && (!trip2)) run_main();
          if (data.status_direct == LOW && !trip3) on_direct();
        } else if (data.status_btn_mid == b2) {
          if ((Irms3 == 0) && (!trip3)) run_main();
          if (data.status_direct == LOW && !trip2) on_direct();
        }
      }
    }
    if (data.start_time > data.stop_time) {  //Nếu thời gian nghỉ nhỏ hơn thời gian chạy
      if ((nowtime > data.stop_time) && (nowtime < data.start_time)) {
        if (data.check_changeday == 0) {
          data.check_changeday = 1;
          data.status_btn_mid = !data.status_btn_mid;
          if (data.status_direct == HIGH) off_direct();
          run_main();
        } else {
          if (data.status_direct == HIGH) off_direct();
          if (data.status_btn_mid == b1) {
            if ((Irms2 == 0) && (!trip2)) run_main();
          } else if (data.status_btn_mid == b2) {
            if ((Irms3 == 0) && (!trip3)) run_main();
          }
        }
      }
      if ((nowtime > data.start_time) || (nowtime < data.stop_time)) {
        if (data.check_changeday == 1) {
          data.check_changeday = 0;
          savedata();
        }
        if (data.status_btn_mid == b1) {
          if ((Irms2 == 0) && (!trip2)) run_main();
          if (data.status_direct == LOW && !trip3) on_direct();
        } else if (data.status_btn_mid == b2) {
          if ((Irms3 == 0) && (!trip3)) run_main();
          if (data.status_direct == LOW && !trip2) on_direct();
        }
      }
    }
  }
  //Cảnh báo áp lực thấp
  if (Result1 >= data.pre_min) i = 0;
  else if (Result1 < data.pre_min) {
    i++;
    if ((i > 3) && (noti_1) && (keynoti)) {  //60s
      Blynk.logEvent("info", String("Áp lực thấp: ") + Result1 + String(" bar"));
      noti_1 = false;
      timeout.setTimeout(30 * m, []() {
        noti_1 = true;
        if (Result1 >= data.pre_min) i = 0;
      });
    }
  }
  //Cảnh báo cấp 1 không chạy
  if ((smoothDistance < data.tank_min) && (keynoti)) {
    j++;
    if (j > 3) {
      if (Irms0 == 0 && !trip0 && Irms1 == 0 && !trip1 && noti_2) {
        noti_2 = false;
        Blynk.logEvent("info", String("Mực nước thấp nhưng Cấp 1 KHÔNG chạy."));
        timeout.setTimeout(60 * m, []() {
          noti_2 = true;
        });
      } else if ((Irms0 == 0 && !trip0) && noti_2) {
        noti_2 = false;
        Blynk.logEvent("info", String("Mực nước thấp nhưng Giếng-1 KHÔNG chạy."));
        timeout.setTimeout(60 * m, []() {
          noti_2 = true;
        });
      } else if ((Irms1 == 0 && !trip1) && noti_2) {
        noti_2 = false;
        Blynk.logEvent("info", String("Mực nước thấp nhưng Giếng-2 KHÔNG chạy."));
        timeout.setTimeout(60 * m, []() {
          noti_2 = true;
        });
      }
    }
  }
}
//-------------------------------------------------------------------
BLYNK_WRITE(V0)  // Bơm 1
{
  if ((key) && (data.mode_cap2 == 1)) {
    if (param.asInt() == HIGH) {
      if ((Irms2 == 0) && (!trip2)) {
        if (Irms3 == 0) {
          data.status_btn_mid = b1;
          run_main();
        } else {
          if (data.status_direct == LOW) on_direct();
        }
      }
    } else {
      if (data.status_btn_mid == b1) {
        off_main();
      } else off_direct();
    }
  } else {
    if ((data.status_btn_mid == b1) && (data.status_btn_left == HIGH)) Blynk.virtualWrite(V0, HIGH);
    else if ((data.status_btn_mid == b2) && (data.status_btn_left == HIGH) && (data.status_direct = HIGH)) Blynk.virtualWrite(V0, HIGH);
    else Blynk.virtualWrite(V0, LOW);
  }
}
BLYNK_WRITE(V1)  // Bơm 2
{
  if ((key) && (data.mode_cap2 == 1)) {
    if (param.asInt() == HIGH) {
      if ((Irms3 == 0) && (!trip3)) {
        if (Irms2 == 0) {
          data.status_btn_mid = b2;
          run_main();
        } else {
          if (data.status_direct == LOW) on_direct();
        }
      }
    } else {
      if (data.status_btn_mid == b2) {
        off_main();
      } else off_direct();
    }
  } else {
    if ((data.status_btn_mid == b2) && (data.status_btn_left == HIGH)) Blynk.virtualWrite(V1, HIGH);
    else if ((data.status_btn_mid == b1) && (data.status_btn_left == HIGH) && (data.status_direct == HIGH)) Blynk.virtualWrite(V1, HIGH);
    else Blynk.virtualWrite(V1, LOW);
  }
}
BLYNK_WRITE(V2)  // Giếng 1
{
  if (key) {
    if ((param.asInt() == HIGH) && (!trip0)) {
      on_G1();
    } else off_G1();
  } else {
    Blynk.virtualWrite(V2, status_g1);
  }
}
BLYNK_WRITE(V3)  // Giếng 2
{
  if (key) {
    if ((param.asInt() == HIGH) && (!trip1)) {
      on_G2();
    } else off_G2();
  } else {
    Blynk.virtualWrite(V3, status_g2);
  }
}
BLYNK_WRITE(V4)  // Chọn chế độ Cấp 2
{
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Tắt máy
          data.mode_cap2 = 0;
          off_main();
          off_direct();
          break;
        }
      case 1:
        {  // Man
          data.mode_cap2 = 1;
          Blynk.setProperty(V0, "isHidden", false);
          Blynk.setProperty(V1, "isHidden", false);
          break;
        }
      case 2:
        {  // Auto
          data.mode_cap2 = 2;
          Blynk.setProperty(V0, "isHidden", true);
          Blynk.setProperty(V1, "isHidden", true);
          break;
        }
    }
  } else
    Blynk.virtualWrite(V4, data.mode_cap2);
}
BLYNK_WRITE(V5)  // Chon máy cài đặt bảo vệ
{
  switch (param.asInt()) {
    case 0:
      {  // ...
        c = 0;
        Blynk.virtualWrite(V6, 0);
        Blynk.virtualWrite(V7, 0);
        break;
      }
    case 1:
      {  // Cấp 1 - 1
        c = 1;
        Blynk.virtualWrite(V6, data.SetAmpemin);
        Blynk.virtualWrite(V7, data.SetAmpemax);
        break;
      }
    case 2:
      {  // Cấp 1 - 2
        c = 2;
        Blynk.virtualWrite(V6, data.SetAmpe1min);
        Blynk.virtualWrite(V7, data.SetAmpe1max);
        break;
      }
    case 3:
      {  // Cap 2 - 1
        c = 3;
        Blynk.virtualWrite(V6, data.SetAmpe2min);
        Blynk.virtualWrite(V7, data.SetAmpe2max);
        break;
      }
    case 4:
      {  // Cấp 2 - 2
        c = 4;
        Blynk.virtualWrite(V6, data.SetAmpe3min);
        Blynk.virtualWrite(V7, data.SetAmpe3max);
        break;
      }
    case 5:
      {  // Pressure Min
        c = 5;
        Blynk.virtualWrite(V6, data.pre_min);
        Blynk.virtualWrite(V7, 0);
        break;
      }
    case 6:
      {  // Tank Min
        c = 6;
        Blynk.virtualWrite(V6, data.tank_min);
        Blynk.virtualWrite(V7, 0);
        break;
      }
  }
}
BLYNK_WRITE(V6)  // min
{
  if (key) {
    if (c == 1) {
      data.SetAmpemin = param.asInt();
    } else if (c == 2) {
      data.SetAmpe1min = param.asInt();
    } else if (c == 3) {
      data.SetAmpe2min = param.asInt();
    } else if (c == 4) {
      data.SetAmpe3min = param.asInt();
    } else if (c == 5) {
      data.pre_min = param.asFloat();
    } else if (c == 6) {
      data.tank_min = param.asInt();
    }
  } else {
    Blynk.virtualWrite(V6, 0);
  }
}
BLYNK_WRITE(V7)  // max
{
  if (key) {
    if (c == 1) {
      data.SetAmpemax = param.asInt();
    } else if (c == 2) {
      data.SetAmpe1max = param.asInt();
    } else if (c == 3) {
      data.SetAmpe2max = param.asInt();
    } else if (c == 4) {
      data.SetAmpe3max = param.asInt();
    }
  } else {
    Blynk.virtualWrite(V7, 0);
  }
}
BLYNK_WRITE(V8)  // info
{
  if (param.asInt() == 1) {
    terminal.clear();
    if (data.mode_cap2 == 1) {
      Blynk.virtualWrite(V10, "MODE: MAN");
    } else if (data.mode_cap2 == 2) {
      int hour_start = data.start_time / 3600;
      int minute_start = (data.start_time - (hour_start * 3600)) / 60;
      int hour_stop = data.stop_time / 3600;
      int minute_stop = (data.stop_time - (hour_stop * 3600)) / 60;
      Blynk.virtualWrite(V10, "MODE: AUTO\nThời gian chạy 2 bơm: ", hour_start, ":", minute_start, " -> ", hour_stop, " : ", minute_stop);
    }
  } else {
    terminal.clear();
  }
  timer.restartTimer(timer_1);
  timer.restartTimer(timer_2);
}
BLYNK_WRITE(V10)  // String
{
  String dataS = param.asStr();
  if (dataS == "mh" || dataS == "MH") {
    terminal.clear();
    key = true;
    Blynk.virtualWrite(V10, "Đơn vị vận hành: CN-Mộc Hóa\nKích hoạt trong 15s\n");
    timeout.setTimeout(15000, []() {
      key = false;
      terminal.clear();
    });
  } else if (dataS == "active") {
    terminal.clear();
    key = true;
    visible();
    Blynk.virtualWrite(V10, "KHÔNG sử dụng phần mềm cho đến khi thông báo này mất.\n");
  } else if (dataS == "deactive") {
    terminal.clear();
    key = false;
    hidden();
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
    on_G1();
    on_G2();
    Blynk.virtualWrite(V10, "Đã RESET! \nNhập mã để điều khiển!\n");
  } else if (dataS == "save_num") {
    terminal.clear();
    Blynk.virtualWrite(V10, "Số lần ghi EEPROM: ", data.save_num);
  } else if (dataS == "rst") {
    terminal.clear();
    Blynk.virtualWrite(V10, "ESP khởi động lại sau 3s");
    reset_4067();
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
  switch (param.asInt()) {
    case 0:
      {  // ...
        b = 0;
        Blynk.virtualWrite(V18, 0);
        break;
      }
    case 1:
      {
        b = 1;
        Blynk.virtualWrite(V18, data.start_time, data.stop_time, tz);
        break;
      }
  }
}
BLYNK_WRITE(V13)  // Bảo vệ
{
  if (key) {
    int data13 = param.asInt();
    if (data13 == LOW) {
      keyp = false;
    } else {
      keyp = true;
    }
  } else
    Blynk.virtualWrite(V13, keyp);
}
BLYNK_WRITE(V17)  // Thông báo
{
  if (key) {
    int data17 = param.asInt();
    if (data17 == LOW) {
      keynoti = false;
    } else {
      keynoti = true;
    }
  } else
    Blynk.virtualWrite(V17, keynoti);
}
BLYNK_WRITE(V18)  // Time input
{
  if (key) {
    TimeInputParam t(param);
    if (t.hasStartTime()) {
      if (b == 1) {
        data.start_time = t.getStartHour() * 3600 + t.getStartMinute() * 60;
      }
    }
    if (t.hasStopTime()) {
      if (b == 1) {
        data.stop_time = t.getStopHour() * 3600 + t.getStopMinute() * 60;
      }
    }
  } else
    Blynk.virtualWrite(V18, 0);
}
//-------------------------------------------------------------------
void up() {
  String server_path = server_name + "batch/update?token=" + BLYNK_AUTH_TOKEN
                       + "&V14=" + Result1
                       + "&V21=" + Irms1
                       + "&V22=" + Irms2
                       + "&V23=" + Irms3
                       + "&V25=" + volume1
                       + "&V26=" + smoothDistance;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  http.end();
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
    Serial.print("data.reboot_num: ");
    Serial.println(data.reboot_num);
    if ((data.reboot_num == 1) || (data.reboot_num == 2)) {
      Serial.println("Restart...");
      Serial.print("data.reboot_num: ");
      Serial.println(data.reboot_num);
      delay(1000);
      ESP.restart();
    }
    if (data.reboot_num % 5 == 0) {
      Serial.print("data.reboot_num: ");
      Serial.println(data.reboot_num);
      delay(1000);
      ESP.restart();
    }
  }
  if (Blynk.connected()) {
    if (data.reboot_num != 0) {
      data.reboot_num = 0;
      savedata();
    }
    Serial.print("data.reboot_num: ");
    Serial.println(data.reboot_num);
    Serial.println("Blynk OK");
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

  emon1.current(A0, 112);
  emon2.current(A0, 112);
  emon3.current(A0, 112);

  eeprom.initialize();
  eeprom.readBytes(address, sizeof(dataDefault), (byte*)&data);
  rtc_module.begin();

  pcf8575.begin();
  pcf8575.pinMode(btn_left, OUTPUT);
  pcf8575.digitalWrite(btn_left, !data.status_btn_left);
  pcf8575.pinMode(btn_mid, OUTPUT);
  pcf8575.digitalWrite(btn_mid, data.status_btn_mid);
  pcf8575.pinMode(btn_right, OUTPUT);
  pcf8575.digitalWrite(btn_right, !data.status_direct);
  pcf8575.pinMode(btn_g1, OUTPUT);
  pcf8575.digitalWrite(btn_g1, HIGH);
  pcf8575.pinMode(btn_g2, OUTPUT);
  pcf8575.digitalWrite(btn_g2, HIGH);
  pcf8575.pinMode(pin4067, OUTPUT);
  pcf8575.digitalWrite(pin4067, LOW);
  pcf8575.pinMode(pin_usb, OUTPUT);
  pcf8575.pinMode(P2, OUTPUT);

  terminal.clear();
  //---------------------------------------------------------------
  timeout.setTimeout(5000, []() {
    timer_1 = timer.setInterval(1183L, []() {
      //readPower();
      readPower1();
      readPower2();
      readPower3();
      up();
      timer.restartTimer(timer_1);
      timer.restartTimer(timer_2);
    });
    timer_2 = timer.setInterval(251L, []() {
      readPressure();
      MeasureCmForSmoothing();
    });
    //-------------------------------
    timer.setInterval(30016L, []() {
      rtctime();
      timer.restartTimer(timer_1);
      timer.restartTimer(timer_2);
    });
    timer.setInterval(900005L, []() {
      connectionstatus();
      timer.restartTimer(timer_1);
      timer.restartTimer(timer_2);
    });
  });
}

void loop() {
  Blynk.run();
  timeout.run();
  timer.run();
}
