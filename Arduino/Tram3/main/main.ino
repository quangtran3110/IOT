/*-Color = 9900FF
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
 *V16 - hidden/visible
 *V17 - info
 *V18 - Btn Nen Khi
 *V19 - I2 - Nen Khi
 *V20 - date/time
 *V21 - status pre
*V22 - Do sau
*V23 - Dung tich
*V24 -
*V25 -
*/

#define BLYNK_TEMPLATE_ID "TMPLnbfxczCv"
#define BLYNK_TEMPLATE_NAME "Trạm Số 3"
#define BLYNK_AUTH_TOKEN "lvmyPh1nGcSjs3n1CTJrX4DyuWLlS0i0"
#define BLYNK_FIRMWARE_VERSION "23.9.1"
#define APP_DEBUG
const char* ssid = "TTTV Xay Dung";
const char* password = "0723841249";
//------------------
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
//------------------
#include <Adafruit_MCP23X17.h>
Adafruit_MCP23X17 mcp;
//------------------
#include "EmonLib.h"
EnergyMonitor emon0, emon1, emon2;
//------------------
#include <WidgetRTC.h>
#include "RTClib.h"
#include <TimeLib.h>
rtc_moduleDS3231 rtc_module;
//------------------
#include <Eeprom24C32_64.h>
#define EEPROM_ADDRESS 0x57
static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);
//------------------
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_main = "http://sgp1.blynk.cloud/external/api/";
String Main = "lvmyPh1nGcSjs3n1CTJrX4DyuWLlS0i0";
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/work/kwaco/tram3/main/main.ino.bin"
//------------------
#define filterSamples 121

const int S0pin = 0;
const int S1pin = 2;
const int S2pin = 14;
const int S3pin = 12;
const word address = 0;
const int relay2 = 8;
const int relay1 = 10;
const int pincap1 = 11;
const int pinbom = 9;
const int btncap1 = 14;
const int btncap2 = 13;
const int pinnenkhi = 100;
long t;
char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
char tz[] = "Asia/Ho_Chi_Minh";
bool key = false, keySet = false, keyp = true, keynoti = true, keytank = true;
bool trip0 = false, trip1 = false, trip2 = false, trip_mcp = false;
bool key_memory = true, timer_I_status;
bool status_pre = false, time_off_c1 = true;
bool noti_cap1 = true, maxtank = false, blynk_first_connect = false;
int c, b, i = 0;
int RelayState, RelayState1, RelayState2;
int timer_1, timer_2, timer_3, timer_4, timer_5, timer_6, timer_7;
int xSetAmpe = 0, xSetAmpe1 = 0, xSetAmpe2 = 0;
int status_cap1 = HIGH, status_bom, status_nenkhi;
int btnState = HIGH, btnState1 = HIGH;
float Irms0, Irms1, Irms2, value, Result1, conlai;
unsigned long int xIrms0 = 0, xIrms1 = 0, xIrms2 = 0;
unsigned long int yIrms0 = 0, yIrms1 = 0, yIrms2 = 0;
unsigned long rest_time = 0, dem_bom = 0;

struct Data {
  byte SetAmpemax, SetAmpemin;
  byte SetAmpe1max, SetAmpe1min;
  byte SetAmpe2max, SetAmpe2min;
  byte mode_cap2, mode_run;
  int bom_chanle_start, bom_chanle_stop;
  int bom_moingay_start, bom_moingay_stop;
  int save_num;
} data, dataCheck;
const struct Data dataDefault = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

WidgetRTC rtc;
WidgetTerminal terminal(V11);
BlynkTimer timer, timer1;
BLYNK_CONNECTED() {
  rtc.begin();
  blynk_first_connect = true;
  Blynk.virtualWrite(V0, status_cap1);
  Blynk.virtualWrite(V1, status_bom);
  Blynk.virtualWrite(V7, data.mode_cap2);
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
void on_cap1() {
  status_cap1 = HIGH;
  mcp.digitalWrite(pincap1, status_cap1);
  Blynk.virtualWrite(V0,status_cap1 );
}
void off_cap1() {
  status_cap1 = LOW;
  mcp.digitalWrite(pincap1, status_cap1);
  Blynk.virtualWrite(V0,status_cap1 );
}
void on_bom() {
  status_bom = HIGH;
  mcp.digitalWrite(pinbom, !status_bom);
  Blynk.virtualWrite(V1, status_bom);
}
void off_bom() {
  status_bom = LOW;
  mcp.digitalWrite(pinbom, !status_bom);
  Blynk.virtualWrite(V1, status_bom);
}
void on_nenkhi() {
  status_nenkhi = HIGH;
  //mcp.digitalWrite(pinnenkhi, !status_nenkhi);
  on_cap1();
}
void off_nenkhi() {
  status_nenkhi = LOW;
  //mcp.digitalWrite(pinnenkhi, !status_nenkhi);
  off_cap1();
}
void hidden() {
  //Blynk.setProperty(V7, "isHidden", true);
  Blynk.setProperty(V12, "isHidden", true);
  Blynk.setProperty(V13, "isHidden", true);
  Blynk.setProperty(V15, "isHidden", true);
  Blynk.setProperty(V14, "isHidden", true);
  Blynk.setProperty(V10, "isHidden", true);
  Blynk.setProperty(V9, "isHidden", true);
  Blynk.setProperty(V8, "isHidden", true);
}
void visible() {
  //Blynk.setProperty(V7, "isHidden", false);
  Blynk.setProperty(V12, "isHidden", false);
  Blynk.setProperty(V13, "isHidden", false);
  Blynk.setProperty(V15, "isHidden", false);
  Blynk.setProperty(V14, "isHidden", false);
  Blynk.setProperty(V10, "isHidden", false);
  Blynk.setProperty(V9, "isHidden", false);
  Blynk.setProperty(V8, "isHidden", false);
}
void off_time() {
  time_off_c1 = true;
  if (Irms1 != 0) {
    off_bom();
  }
}
void on_time() {
  time_off_c1 = false;
  if ((!trip1) && (Irms1 == 0)) {
    on_bom();
  }
  if ((!trip0) && (Irms0 == 0)) {
    on_cap1();
  }
}
void off_time_cap1() {
  time_off_c1 = true;
  if (Irms0 != 0) {
    off_cap1();
  }
}
//-------------------------------------------------------------------
void readPower()  // C2 - Cấp 1  - I0
{
  //Blynk.run();
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float rms0 = emon0.calcIrms(740);
  if (rms0 < 2) {
    Irms0 = 0;
    yIrms0 = 0;
    if ((noti_cap1) && (status_pre) && (conlai <= 80) && (!time_off_c1)) {
      if (keynoti) {
        noti_cap1 = false;
        Blynk.logEvent("info", String("Bơm Giếng không chạy. Xin kiểm tra."));
      }
    }
  } else if (rms0 >= 2) {
    yIrms0 = yIrms0 + 1;
    Irms0 = rms0;
    if (yIrms0 > 3) {
      if ((Irms0 >= data.SetAmpemax) || (Irms0 <= data.SetAmpemin)) {
        xSetAmpe = xSetAmpe + 1;
        if ((xSetAmpe > 3) && (keyp)) {
          off_cap1();
          xSetAmpe = 0;
          trip0 = true;
          if (keynoti) {
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
void readPower1()  // C3 - Bơm    - I1
{
  //Blynk.run();
  digitalWrite(S0pin, HIGH);
  digitalWrite(S1pin, HIGH);
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW);
  float rms1 = emon1.calcIrms(740);
  if (rms1 < 2) {
    Irms1 = 0;
    yIrms1 = 0;
  } else if (rms1 >= 2) {
    Irms1 = rms1;
    yIrms1 = yIrms1 + 1;
    if (yIrms1 > 3) {
      if ((Irms1 >= data.SetAmpe1max) || (Irms1 <= data.SetAmpe1min)) {
        xSetAmpe1 = xSetAmpe1 + 1;
        if ((xSetAmpe1 > 3) && (keyp)) {
          off_bom();
          xSetAmpe1 = 0;
          trip1 = true;
          if (keynoti) {
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
void readPower2()  //C4 - Nen khi- I2
{
  //Blynk.run();
  digitalWrite(S0pin, LOW);
  digitalWrite(S1pin, LOW);
  digitalWrite(S2pin, HIGH);
  digitalWrite(S3pin, LOW);
  float rms2 = emon2.calcIrms(740);
  if (rms2 < 2) {
    Irms2 = 0;
    yIrms2 = 0;
  } else if (rms2 >= 2) {
    Irms2 = rms2;
    yIrms2 = yIrms2 + 1;
    if ((yIrms2 > 3) && ((Irms2 >= data.SetAmpe2max) || (Irms2 <= data.SetAmpe2min))) {
      xSetAmpe2 = xSetAmpe2 + 1;
      if ((xSetAmpe2 > 3) && (keyp)) {
        off_nenkhi();
        xSetAmpe2 = 0;
        trip2 = true;
        trip1 = true;
        if (keynoti) {
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
void rtctime() {
  DateTime now = rtc_module.now();
  if (blynk_first_connect == true) {
    if ((now.day() != day()) || (now.hour() != hour()) || ((now.minute() - minute() > 2) || (minute() - now.minute() > 2))) {
      rtc_module.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
      DateTime now = rtc_module.now();
    }
  }
  Blynk.virtualWrite(V20, daysOfTheWeek[now.dayOfTheWeek()], ", ", now.day(), "/", now.month(), "/", now.year(), " - ", now.hour(), ":", now.minute(), ":", now.second());

  int nowtime = (now.hour() * 3600 + now.minute() * 60);

  if (data.mode_cap2 == 1) {   // Chạy Tu Dong
    if (data.mode_run == 0) {  //Ngay chan tat may
      if (now.day() % 2 == 0) {
        if ((nowtime + 1800) > data.bom_chanle_stop) {  //30p
          off_time_cap1();
          trip0 = true;
          if (nowtime > data.bom_chanle_stop) {
            trip0 = false;
            off_time();
          }
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
        if ((nowtime + 1200) > data.bom_chanle_stop) {  //20p
          off_time_cap1();
          trip0 = true;
          if (nowtime > data.bom_chanle_stop) {
            off_time();
            trip0 = false;
          }
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
        if (((nowtime + 1800) > data.bom_moingay_stop) && (nowtime < data.bom_moingay_start)) {
          off_time_cap1();
          trip0 = true;
          if (!maxtank && ((nowtime > data.bom_moingay_stop) && (nowtime < data.bom_moingay_start))) {
            off_time();
            trip0 = false;
          }
        }
        if ((nowtime < data.bom_moingay_stop) || (nowtime > data.bom_moingay_start) || maxtank) {
          on_time();
        }
      }
      if (data.bom_moingay_start < data.bom_moingay_stop) {
        if (((nowtime + 1800) > data.bom_moingay_stop) || (nowtime < data.bom_moingay_start)) {
          off_time_cap1();
          trip0 = true;
          if (!maxtank && ((nowtime > data.bom_moingay_stop) || (nowtime < data.bom_moingay_start))) {
            off_time();
            trip0 = false;
          }
        }
        if (((nowtime < data.bom_moingay_stop) && (nowtime > data.bom_moingay_start)) || maxtank) {
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
    RelayState = param.asInt();
    if (RelayState == LOW) {
      off_cap1();
    } else {
      on_cap1();
    }
  }
  Blynk.virtualWrite(V0, status_cap1);
}
BLYNK_WRITE(V1)  // Bơm 2
{
  if ((key) && (!trip1)) {
    RelayState1 = param.asInt();
    if (RelayState1 == LOW) {
      off_bom();
    } else {
      on_bom();
    }
  }
  Blynk.virtualWrite(V1, status_bom);
}
BLYNK_WRITE(V5)  // Con lai
{
  conlai = param.asFloat();
  if ((conlai > 220) && (status_pre)) {
    maxtank = true;
    if (keytank) {
      Blynk.logEvent("info", String("Nước trong bể cao vượt mức ") + (conlai - 220) + String(" cm"));
      keytank = false;
      timer1.setTimeout(600000L, []() {
        keytank = true;
      });
    }
  }
  if ((conlai < 200) && (status_pre)) {
    maxtank = false;
  }
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
      {  // ....
        c = 0;
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
  if (keySet) {
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
  if (keySet) {
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
  if (dataS == "ts3") {
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
    keySet = true;
    Blynk.virtualWrite(V11, "KHÔNG sử dụng phần mềm cho đến khi thông báo này mất.\n");
  } else if (dataS == "deactive") {
    terminal.clear();
    key = false;
    keySet = false;
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
    on_cap1();
    Blynk.virtualWrite(V11, "Đã RESET! \nNhập mã để điều khiển!\n");
  } else if (dataS == "rst") {
    terminal.clear();
    Blynk.virtualWrite(V11, "ESP khởi động lại sau 3s");
    delay(3000);
    ESP.restart();
  } else if (dataS == "update") {
    terminal.clear();
    Blynk.virtualWrite(V11, "UPDATE FIRMWARE...");
    update_fw();
  } else {
    Blynk.virtualWrite(V11, "Mật mã sai.\nVui lòng nhập lại!\n");
  }
}
BLYNK_WRITE(V12)  // Chọn thời gian chạy Bơm
{
  switch (param.asInt()) {
    case 0:
      {
        if (key) {
          data.mode_run = 0;  //Chẵn
        }
        Blynk.virtualWrite(V13, data.bom_chanle_start, data.bom_chanle_stop, tz);
        break;
      }
    case 1:
      {
        if (key)
          data.mode_run = 1;  //Lẻ
        Blynk.virtualWrite(V13, data.bom_chanle_start, data.bom_chanle_stop, tz);
        break;
      }
    case 2:
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
  if (keySet) {
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
  if (keySet) {
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
  if (keySet) {
    int data16 = param.asInt();
    if (data16 == LOW) {
      keynoti = false;
    } else {
      keynoti = true;
    }
  } else
    Blynk.virtualWrite(V15, keynoti);
}
BLYNK_WRITE(V16)  // Hidden/Visible
{
  if (param.asInt() == 0) {
    hidden();
  } else
    visible();
}
BLYNK_WRITE(V17)  // Info
{
  if (param.asInt() == 1) {
    terminal.clear();
    if (data.mode_cap2 == 0) {
      Blynk.virtualWrite(V11, "Mode: MAN");
    } else if (data.mode_cap2 == 1) {
      if (data.mode_run == 0) {  //Chẵn
        int chanle_start_h = data.bom_chanle_start / 3600;
        int chanle_start_m = (data.bom_chanle_start - (chanle_start_h * 3600)) / 60;
        int chanle_stop_h = data.bom_chanle_stop / 3600;
        int chanle_stop_m = (data.bom_chanle_stop - (chanle_stop_h * 3600)) / 60;
        Blynk.virtualWrite(V11, "Mode: AUTO - Chẵn\nThời gian nghỉ: ", chanle_stop_h, " : ", chanle_stop_m, " -> ", chanle_start_h, " : ", chanle_start_m);
      }
      if (data.mode_run == 1) {  //Lẻ
        int chanle_start_h = data.bom_chanle_start / 3600;
        int chanle_start_m = (data.bom_chanle_start - (chanle_start_h * 3600)) / 60;
        int chanle_stop_h = data.bom_chanle_stop / 3600;
        int chanle_stop_m = (data.bom_chanle_stop - (chanle_stop_h * 3600)) / 60;
        Blynk.virtualWrite(V11, "Mode: AUTO - Lẻ\nThời gian nghỉ: ", chanle_stop_h, " : ", chanle_stop_m, " -> ", chanle_start_h, " : ", chanle_start_m);
      }
      if (data.mode_run == 2) {  //Mỗi ngày
        int moingay_start_h = data.bom_moingay_start / 3600;
        int moingay_start_m = (data.bom_moingay_start - (moingay_start_h * 3600)) / 60;
        int moingay_stop_h = data.bom_moingay_stop / 3600;
        int moingay_stop_m = (data.bom_moingay_stop - (moingay_stop_h * 3600)) / 60;
        Blynk.virtualWrite(V11, "Mode: AUTO - Mỗi ngày\nThời gian nghỉ: ", moingay_stop_h, " : ", moingay_stop_m, " -> ", moingay_start_h, " : ", moingay_start_m);
      }
    }
  } else {
    terminal.clear();
  }
  timer.restartTimer(timer_1);
  timer.restartTimer(timer_2);
}
BLYNK_WRITE(V18)  // Nen Khi
{
  if ((key) && (!trip2)) {
    RelayState2 = param.asInt();
    if (RelayState2 == LOW) {
      off_nenkhi();
    } else on_nenkhi();
  }
  Blynk.virtualWrite(V18, status_nenkhi);
}
BLYNK_WRITE(V21)  //Status_pre
{
  status_pre = param.asInt();
}
//-------------------------------------------------------------------
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
//-------------------------
void updata() {
  String server_path = server_main + "batch/update?token=" + Main
                       //+ "&V0=" + byte(status_cap1)
                       //+ "&V1=" + byte(status_bom)
                       + "&V2=" + Irms0
                       + "&V3=" + Irms1
                       //+ "&V7=" + byte(data.mode_cap2)
                       //+ "&V14=" + byte(keyp)
                       //+ "&V15=" + byte(keynoti)
                       //+ "&V18=" + byte(status_nenkhi)
                       + "&V19=" + Irms2;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  http.end();
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

  emon0.current(A0, 130);
  emon1.current(A0, 130);
  emon2.current(A0, 120);

  if (!mcp.begin_I2C()) {
    trip_mcp = true;
  }

  mcp.pinMode(6, INPUT);
  mcp.pinMode(7, INPUT);
  mcp.pinMode(pincap1, OUTPUT);
  mcp.digitalWrite(pincap1, HIGH);  // gieng
  mcp.pinMode(pinbom, OUTPUT);
  mcp.digitalWrite(pinbom, HIGH);  // Bom
  //mcp.pinMode(pinnenkhi, OUTPUT); mcp.digitalWrite(pinnenkhi, HIGH);

  rtc_module.begin();
  //rtc.adjust(DateTime(2021, 12, 22, 9, 59, 0));
  eeprom.initialize();
  eeprom.readBytes(address, sizeof(dataDefault), (byte*)&data);

  terminal.clear();
  //------------------------------------
  setSyncInterval(10 * 60);
  timer.setTimeout(5000L, []() {
    timer_1 = timer1.setInterval(1083L, []() {
      readPower();
      readPower1();
      readPower2();
      updata();
      timer1.restartTimer(timer_1);
    });
    timer_2 = timer1.setInterval(15006L, []() {
      rtctime();
      timer1.restartTimer(timer_1);
    });
  });
}

void loop() {
  Blynk.run();
  timer.run();
  timer1.run();
}
