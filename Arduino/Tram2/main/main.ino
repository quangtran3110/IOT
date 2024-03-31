/* V0-chọn người vận hành
   V1-date/time
   V2-
   V3-
   V4-on/off bao ve ampe
   V5-keyterminal
   V6-
   V7-
   V8-
   V9-
   V10-Áp suất
   V11-Chọn máy để cài bảo vệ
   V12-Chọn giá trị bảo vệ Min
   V13-Chọn giá trị bảo vệ Max
   V14-ip Wifi
   V15 - on Bom 30kw
   V16 - off bom 30kw
   V17 - 
   V18 -
   V19 - 
   V20 - ONG3
   ...
   V32- I7
   V33-
   V34-con lai
   V35-volume
   V36- I8
   V37-dosau
   V38-statusPRE
   V39-statusRL
   V40-I0
   ...
   V46-I6
   V47- btn nén khí 1
   V48- btn nén khí 2
   V49-dungtich
   V50-Terminal Luu luong
   V51-input luu luong
   V52-check luu luong
   V53-
   V54-Luu Luong G1
   V55-Luu Luong G2
   V56-Khoi luong Clo
   V57-
   V58-G2 LL1m3
   V59-G2 LL24h
   V60-statusVLG2
   V61-G1 LL24h
   V62-G1 LL1m3
   V63-statusVLG1
   V64-LLG1_RL
   V65-LLG2_RL
*/
#define BLYNK_TEMPLATE_ID "TMPLK0N90h0w"
#define BLYNK_TEMPLATE_NAME "Trạm Số 2"
#define BLYNK_AUTH_TOKEN "ESzia3fpA-29cs8gt85pGnrPq_rICcqf"

#define BLYNK_FIRMWARE_VERSION "240228"
#define BLYNK_PRINT Serial
#define APP_DEBUG

//-----------------------------
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
const char* ssid = "Hiddennet";
const char* password = "Password";
//-----------------------------
#include "EmonLib.h"
EnergyMonitor emon0, emon1, emon2, emon3, emon4, emon5, emon6, emon7, emon8;
int xSetAmpe = 0, xSetAmpe1 = 0, xSetAmpe2 = 0, xSetAmpe3 = 0, xSetAmpe4 = 0, xSetAmpe5 = 0, xSetAmpe6 = 0, xSetAmpe7 = 0, xSetAmpe8 = 0;
unsigned long int yIrms0 = 0, yIrms1 = 0, yIrms2 = 0, yIrms3 = 0, yIrms4 = 0, yIrms5 = 0, yIrms6 = 0, yIrms7 = 0, yIrms8 = 0, dem1 = 0, dem2 = 0, dem3 = 0;
float Irms0, Irms1, Irms2, Irms3, Irms4, Irms5, Irms6, Irms7, Irms8, pre;
bool trip0 = false, trip1 = false, trip2 = false, trip3 = false, trip4 = false, trip5 = false, trip6 = false, trip7 = false, trip8 = false;
//-----------------------------
#include <WidgetRTC.h>
#include "RTClib.h"
RTC_DS3231 rtc_module;
char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
//-----------------------------
#include <Eeprom24C32_64.h>
#define EEPROM_ADDRESS 0x57
static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);
const word address = 0;
//-----------------------------
#include "PCF8575.h"
PCF8575 pcf8575_1(0x20);
const int pin_on_G1 = P7;
const int pin_off_G1 = P6;
const int pin_on_G2 = P5;
const int pin_off_G2 = P4;
const int pin_on_G3 = P3;
const int pin_off_G3 = P2;
const int pin_NK1 = P1;
const int pin_NK2 = P0;
const int pin_on_Bom1 = P8;
const int pin_off_Bom1 = P9;
const int pin_on_Bom2 = P10;
const int pin_off_Bom2 = P11;
const int pin_on_Bom3 = P12;
const int pin_off_Bom3 = P13;
const int pin_on_Bom4 = P14;
const int pin_off_Bom4 = P15;
//-----------------------------
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_local = "http://blynkkwaco.ddns.net:8080/";
String Tram2_Rualoc = "1nmLSMB-Q2dVjnPGKYbrGaEXcJ_droWB";
String server_name = "http://sgp1.blynk.cloud/external/api/";
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/IOT/main/Arduino/Tram2/main/build/esp8266.esp8266.nodemcuv2/main.ino.bin"
//-----------------------------
const int S0 = 14;
const int S1 = 12;
const int S2 = 13;
const int S3 = 15;
//----------------------------------
const int dai = 2000;
const int rong = 1000;
const int dosau = 515;
#define filterSamples 121
int zeropointTank = 199, fullpointTank = 936;
int volume, volume1, dungtich, smoothDistance;
long distance, distance1;
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
//----------------------------------
bool keySwitchQ = false, keySwitchD = false, keySwitchP = false, keySet = false, data12 = true, data13 = true, keyPRE2 = true, keyPRE4 = true, noti = true;
bool data.protect = true, timer_updata_status, timer_I_status;
bool kdata15 = true, kdata16 = true, kdata20 = true, kdata21 = true, kdata22 = true, kdata23 = true, kdata24 = true, kdata25 = true, kdata26 = true, kdata27 = true, kdata28 = true, kdata29 = true, kdata30 = true, kdata31 = true;
bool event30p = true;
bool blynk_first_connect = false;
byte menu_gieng_luuluong;
unsigned long ll_g1_cache = 0, ll_g2_cache = 0;
float clo_cache = 0;
uint32_t timestamp;
int timer_rtc, timer_I, timer_tank;
int g = 1, h, z, i, j, n, m;
int startt, stopt;

int LLG2_1m3, LLG1_1m3;
//----------------------------------
struct Data {
  byte SetAmpemax, SetAmpemin;
  byte SetAmpe1max, SetAmpe1min;
  byte SetAmpe2max, SetAmpe2min;
  byte SetAmpe3max, SetAmpe3min;
  byte SetAmpe4max, SetAmpe4min;
  byte SetAmpe5max, SetAmpe5min;
  byte SetAmpe6max, SetAmpe6min;
  byte SetAmpe7max, SetAmpe7min;
  byte SetAmpe8max, SetAmpe8min;
  //int startH, startM;
  //int stopH, stopM;
  byte man;
  int save_num;
  byte time_run_nk1, time_stop_nk1, time_run_nk2, time_stop_nk2;
  byte reboot_num, status_rualoc;
  unsigned long ll_g1, ll_g2;
  float clo;
  int time_clo, LLG2_RL, LLG1_RL;
  byte protect;
} data, dataCheck;
const struct Data dataDefault = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

WidgetTerminal keyterminal(V5);
WidgetTerminal terminal_luuluong(V50);
WidgetRTC rtc_widget;
BlynkTimer timer, timer1;
BLYNK_CONNECTED() {
  rtc_widget.begin();
  blynk_first_connect = true;
}

void bridge_Tram2C(String token, int virtual_pin, float(value_to_send)) {
  String server_path = server_local + token + "/update/V" + String(virtual_pin) + "?value=" + value_to_send;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
  }
  http.end();
}
//----------------------------------
void event_30p() {
  if (event30p) {
    event30p = false;
    timer1.setTimeout(300000L, []() {
      event30p = true;
    });
    if (data.man == 1) {
      Blynk.logEvent("D", String("Đã ngưng Giếng 30p.\nBể chứa còn: ") + volume1 + String(" m3 (") + smoothDistance + String(" cm)"));
    } else if (data.man == 2) {
      Blynk.logEvent("G", String("Đã ngưng Giếng 30p.\nBể chứa còn: ") + volume1 + String(" m3 (") + smoothDistance + String(" cm)"));
    } else if (data.man == 3) {
      Blynk.logEvent("Q", String("Đã ngưng Giếng 30p.\nBể chứa còn: ") + volume1 + String(" m3 (") + smoothDistance + String(" cm)"));
    }
  }
}
void event_pressure() {
  if (data.man == 1) {
    Blynk.logEvent("1-al", String("Áp lực hiện tại là ") + pre + String(" bar"));
  } else if (data.man == 2) {
    Blynk.logEvent("2-al", String("Áp lực hiện tại là ") + pre + String(" bar"));
  } else if (data.man == 3) {
    Blynk.logEvent("q-al", String("Áp lực hiện tại là ") + pre + String(" bar"));
  }
}
void savedata() {
  if (memcmp(&data, &dataCheck, sizeof(dataDefault)) == 0) {
    // Serial.println("structures same no need to write to EEPROM");
  } else {
    // Serial.println("\nWrite bytes to EEPROM memory...");
    data.save_num = data.save_num + 1;
    eeprom.writeBytes(address, sizeof(dataDefault), (byte*)&data);
    Blynk.setProperty(V5, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
  }
}
//----------------------------------
void onG1() {
  pcf8575_1.digitalWrite(pin_on_G1, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_on_G1, HIGH);
}
void offG1() {
  pcf8575_1.digitalWrite(pin_off_G1, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_off_G1, HIGH);
}
void onG2() {
  pcf8575_1.digitalWrite(pin_on_G2, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_on_G2, HIGH);
}
void offG2() {
  pcf8575_1.digitalWrite(pin_off_G2, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_off_G2, HIGH);
}
void onG3() {
  pcf8575_1.digitalWrite(pin_on_G3, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_on_G3, HIGH);
}
void offG3() {
  pcf8575_1.digitalWrite(pin_off_G3, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_off_G3, HIGH);
}
//----------------------------------
void on_Bom1() {
  pcf8575_1.digitalWrite(pin_on_Bom1, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_on_Bom1, HIGH);
}
void off_Bom1() {
  pcf8575_1.digitalWrite(pin_off_Bom1, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_off_Bom1, HIGH);
}
void on_Bom2() {
  pcf8575_1.digitalWrite(pin_on_Bom2, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_on_Bom2, HIGH);
}
void off_Bom2() {
  pcf8575_1.digitalWrite(pin_off_Bom2, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_off_Bom2, HIGH);
}
void on_Bom3() {
  pcf8575_1.digitalWrite(pin_on_Bom3, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_on_Bom3, HIGH);
}
void off_Bom3() {
  pcf8575_1.digitalWrite(pin_off_Bom3, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_off_Bom3, HIGH);
}
void on_Bom4() {
  pcf8575_1.digitalWrite(pin_on_Bom4, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_on_Bom4, HIGH);
}
void off_Bom4() {
  pcf8575_1.digitalWrite(pin_off_Bom4, LOW);
  delay(200);
  pcf8575_1.digitalWrite(pin_off_Bom4, HIGH);
}
//----------------------------------
void on_NK1() {
  pcf8575_1.digitalWrite(pin_NK1, HIGH);
}
void off_NK1() {
  pcf8575_1.digitalWrite(pin_NK1, LOW);
}
void on_NK2() {
  pcf8575_1.digitalWrite(pin_NK2, HIGH);
}
void off_NK2() {
  pcf8575_1.digitalWrite(pin_NK2, LOW);
}
//----------------------------------
void hidden() {
  Blynk.setProperty(V11, v12, v13, V4, V8, V9, "isHidden", true);
}
void visible() {
  Blynk.setProperty(V11, v12, v13, V4, V8, V9, "isHidden", false);
}
//----------------------------------
BLYNK_WRITE(V0)  // Nguoi truc
{
  if (keySwitchP || keySwitchD || keySwitchQ) {
    switch (param.asInt()) {
      case 0:
        {  // Duc
          data.man = 1;
          break;
        }
      case 1:
        {  // Phong
          data.man = 2;
          break;
        }
      case 2:
        {  // Quang
          data.man = 3;
          break;
        }
    }
    savedata();
  } else
    Blynk.virtualWrite(V0, data.man - 1);
}
BLYNK_WRITE(V2)  //Rua Loc
{
  if (keySwitchP || keySwitchD || keySwitchQ) {
    switch (param.asInt()) {
      case 0:
        {  //Tắt
          data.status_rualoc = 0;
          if ((data.LLG1_RL != 0) || (data.LLG2_RL != 0)) {
            if (data.LLG1_RL != 0) {
              Blynk.virtualWrite(V64, LLG1_1m3 - data.LLG1_RL);
              data.LLG1_RL = 0;
            }
            if (data.LLG2_RL != 0) {
              Blynk.virtualWrite(V65, LLG2_1m3 - data.LLG2_RL);
              data.LLG2_RL = 0;
            }
            savedata();
          }
          break;
        }
      case 1:
        {  //RL 1-2
          data.status_rualoc = 1;
          if (data.LLG1_RL == 0) {
            data.LLG1_RL = LLG1_1m3;
          }
          if (data.LLG2_RL == 0) {
            data.LLG2_RL = LLG2_1m3;
          }
          break;
        }
      case 2:
        {  //RL 3
          data.status_rualoc = 2;
          break;
        }
      case 3:
        {  //RL 1-2-3
          data.status_rualoc = 3;
          if (data.LLG1_RL == 0) {
            data.LLG1_RL = LLG1_1m3;
          }
          if (data.LLG2_RL == 0) {
            data.LLG2_RL = LLG2_1m3;
          }
          break;
        }
    }
    savedata();
    bridge_Tram2C(Tram2_Rualoc, 0, data.status_rualoc);
  } else {
    Blynk.virtualWrite(V2, data.status_rualoc);
  }
}
/*
BLYNK_WRITE(V3)  // Chon gio Start-Stop
{
  switch (param.asInt()) {
    case 0:
      {  //
        if (keySet) {
          h = 0;
        }
        Blynk.virtualWrite(V6, 0);
        Blynk.virtualWrite(V7, 0);
        break;
      }
    case 1:
      {  // Start
        if (keySet) {
          h = 1;
        }
        Blynk.virtualWrite(V6, data.startH);
        Blynk.virtualWrite(V7, data.startM);
        break;
      }
    case 2:
      {  // Stop
        if (keySet) {
          h = 2;
        }
        Blynk.virtualWrite(V6, data.stopH);
        Blynk.virtualWrite(V7, data.stopM);
        break;
      }
  }
}
*/
BLYNK_WRITE(V4)  // On/off chuc nang bao ve
{
  if (keySet) {
    if (param.asInt() == LOW) {
      data.protect = false;
    } else {
      data.protect = true;
    }
  } else {
    Blynk.virtualWrite(V4, data.protect);
  }
}
BLYNK_WRITE(V5)  // data string
{
  String dataS = param.asStr();
  if ((dataS == "t2")) {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "Người vận hành: 'T.Phong'\nKích hoạt trong 15s\n");
    keySwitchP = true;
    timer.setTimeout(15000, []() {
      keySwitchP = false;
      keyterminal.clear();
    });
  } else if (dataS == "M") {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "Người vận hành: 'M.Quang'\nKích hoạt trong 10s\n");
    keySwitchQ = true;
    timer.setTimeout(10000, []() {
      keySwitchQ = false;
      keyterminal.clear();
    });
  } else if ((dataS == "đ") || (dataS == "Đ")) {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "Người vận hành: 'C.Đức'\nKích hoạt trong 15s\n");
    keySwitchD = true;
    timer.setTimeout(15000, []() {
      keySwitchD = false;
      keyterminal.clear();
    });
  } else if (dataS == "active") {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "Kích hoạt chế độ sửa lỗi!\nKHÔNG sử dụng phần mềm cho đến khi thông báo nào mất!");
    keySwitchQ = true;
    keySet = true;
    visible();
  } else if (dataS == "deactive") {
    keyterminal.clear();
    keySwitchQ = false;
    keySet = false;
    hidden();
    Blynk.virtualWrite(V5, "Hãy nhập mã...!\n");
  } else if (dataS == "save") {
    keyterminal.clear();
    savedata();
    Blynk.virtualWrite(V5, "Đã lưu!\n");
  } else if (dataS == "reset") {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "Đã reset!");
    trip0 = false;
    trip1 = false;
    trip2 = false;
    trip3 = false;
    trip4 = false;
    trip5 = false;
    trip6 = false;
    pcf8575_1.digitalWrite(pin_off_G3, HIGH);    // G3
    pcf8575_1.digitalWrite(pin_off_G2, HIGH);    // G2
    pcf8575_1.digitalWrite(pin_off_G1, HIGH);    // G1
    pcf8575_1.digitalWrite(pin_off_Bom4, HIGH);  // 11kw
    pcf8575_1.digitalWrite(pin_off_Bom3, HIGH);  // 7.5kw
    pcf8575_1.digitalWrite(pin_off_Bom1, HIGH);  // 18.5kw
    pcf8575_1.digitalWrite(pin_off_Bom2, HIGH);  // 30kw
    pcf8575_1.digitalWrite(pin_NK1, HIGH);       // NK1
    pcf8575_1.digitalWrite(pin_NK2, HIGH);       // NK2
  } else if (dataS == "update") {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "UPDATE FIRMWARE...");
    update_fw();
  } else if (dataS == "save_num") {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "Số lần ghi EEPROM: ", data.save_num);
  } else if (dataS == "rst") {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "ESP Khởi động lại sau 3s");
    delay(3000);
    ESP.restart();
  } else {
    Blynk.virtualWrite(V5, "Mã không hợp lệ!\nVui lòng nhập lại.\n");
  }
}
/*
BLYNK_WRITE(V6)  // input gio
{
  if (keySet) {
    int number = param.asInt();
    if (h == 1) {
      data.startH = number;
    } else if (h == 2) {
      data.stopH = number;
    }
  } else {
    Blynk.virtualWrite(V6, 0);
  }
}
BLYNK_WRITE(V7)  // input phut
{
  if (keySet) {
    int number = param.asInt();
    if (h == 1) {
      data.startM = number;
    } else if (h == 2) {
      data.stopM = number;
    }
  } else {
    Blynk.virtualWrite(V7, 0);
  }
}
*/
BLYNK_WRITE(V11)  // Chon máy cài đặt bảo vệ
{
  switch (param.asInt()) {
    case 0:
      {
        z = 0;
        Blynk.virtualWrite(V12, 0);
        Blynk.virtualWrite(V13, 0);
        break;
      }
    case 1:
      {  // 18.5kw
        z = 1;
        Blynk.virtualWrite(V12, data.SetAmpemin);
        Blynk.virtualWrite(V13, data.SetAmpemax);
        break;
      }
    case 2:
      {  // 30k
        z = 2;
        Blynk.virtualWrite(V12, data.SetAmpe6min);
        Blynk.virtualWrite(V13, data.SetAmpe6max);
        break;
      }
    case 3:
      {  // 7.5kw
        z = 3;
        Blynk.virtualWrite(V12, data.SetAmpe4min);
        Blynk.virtualWrite(V13, data.SetAmpe4max);
        break;
      }
    case 4:
      {  // 11kw
        z = 4;
        Blynk.virtualWrite(V12, data.SetAmpe2min);
        Blynk.virtualWrite(V13, data.SetAmpe2max);
        break;
      }
    case 5:
      {  // G1
        z = 5;
        Blynk.virtualWrite(V12, data.SetAmpe3min);
        Blynk.virtualWrite(V13, data.SetAmpe3max);
        break;
      }
    case 6:
      {  // G2
        z = 6;
        Blynk.virtualWrite(V12, data.SetAmpe1min);
        Blynk.virtualWrite(V13, data.SetAmpe1max);
        break;
      }
    case 7:
      {  // G3
        z = 7;
        Blynk.virtualWrite(V12, data.SetAmpe5min);
        Blynk.virtualWrite(V13, data.SetAmpe5max);
        break;
      }
    case 8:
      {  // NK1
        z = 8;
        Blynk.virtualWrite(V12, data.SetAmpe7min);
        Blynk.virtualWrite(V13, data.SetAmpe7max);
        break;
      }
    case 9:
      {  // NK2
        z = 9;
        Blynk.virtualWrite(V12, data.SetAmpe8min);
        Blynk.virtualWrite(V13, data.SetAmpe8max);
        break;
      }
  }
}
BLYNK_WRITE(V12)  // min
{
  if (keySet) {
    if (z == 1) {
      data.SetAmpemin = param.asInt();
    } else if (z == 2) {
      data.SetAmpe6min = param.asInt();
    } else if (z == 3) {
      data.SetAmpe4min = param.asInt();
    } else if (z == 4) {
      data.SetAmpe2min = param.asInt();
    } else if (z == 5) {
      data.SetAmpe3min = param.asInt();
    } else if (z == 6) {
      data.SetAmpe1min = param.asInt();
    } else if (z == 7) {
      data.SetAmpe5min = param.asInt();
    } else if (z == 8) {
      data.SetAmpe7min = param.asInt();
    } else if (z == 9) {
      data.SetAmpe8min = param.asInt();
    }
  } else {
    Blynk.virtualWrite(V12, 0);
  }
}
BLYNK_WRITE(V13)  // max
{
  if (keySet) {
    if (z == 1) {
      data.SetAmpemax = param.asInt();
    } else if (z == 2) {
      data.SetAmpe6max = param.asInt();
    } else if (z == 3) {
      data.SetAmpe4max = param.asInt();
    } else if (z == 4) {
      data.SetAmpe2max = param.asInt();
    } else if (z == 5) {
      data.SetAmpe3max = param.asInt();
    } else if (z == 6) {
      data.SetAmpe1max = param.asInt();
    } else if (z == 7) {
      data.SetAmpe5max = param.asInt();
    } else if (z == 8) {
      data.SetAmpe7max = param.asInt();
    } else if (z == 9) {
      data.SetAmpe8max = param.asInt();
    }
  } else {
    Blynk.virtualWrite(V13, 0);
  }
}
BLYNK_WRITE(V15)  // On Bơm 2 - 30KW
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms6 == 0) && (kdata15)) {
        kdata15 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang mở Bơm 30Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức mở Bơm 30Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong mở Bơm 30Kw"));
          }
          kdata15 = true;
        });
      }
      on_Bom2();
    }
  }
}
BLYNK_WRITE(V16)  // Off 30 KW
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms6 != 0) && (kdata16)) {
        kdata16 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang tắt Bơm 30Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức tắt Bơm 30Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong tắt Bơm 30Kw"));
          }
          kdata16 = true;
        });
      }
      off_Bom2();
    }
  }
}
/*
BLYNK_WRITE(V17)  // info
{
  if (param.asInt() == LOW) {
    keyterminal.clear();
  } else {
    keyterminal.clear();
    Blynk.virtualWrite(V5, "Tắt cấp II lúc:", data.stopH, ":", data.stopM, "\nChạy lại lúc: ", data.startH, ":", data.startM);
  }
}
*/
BLYNK_WRITE(V20)  // On G3
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms5 == 0) && (kdata20)) {
        kdata20 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang mở Giếng 3"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức mở Giếng 3"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong mở Giếng 3"));
          }
          kdata20 = true;
        });
      }
      onG3();
    }
  }
}
BLYNK_WRITE(V21)  // Off G2
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms1 != 0) && (kdata21)) {
        kdata21 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang tắt Giếng 2"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức tắt Giếng 2"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong tắt Giếng 2"));
          }
          kdata21 = true;
        });
      }
      offG2();
    }
  }
}
BLYNK_WRITE(V22)  // Off 11 Kw
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms2 != 0) && (kdata22)) {
        kdata22 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang tắt Bơm 11Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức tắt Bơm 11Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong tắt Bơm 11Kw"));
          }
          kdata22 = true;
        });
      }
      off_Bom4();
    }
  }
}
BLYNK_WRITE(V23)  // Off G1
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms3 != 0) && (kdata23)) {
        kdata23 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang tắt Giếng 1"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức tắt Giếng 1"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong tắt Giếng 1"));
          }
          kdata23 = true;
        });
      }
      offG1();
    }
  }
}
BLYNK_WRITE(V24)  // Off 7.5 KW
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms4 != 0) && (kdata24)) {
        kdata24 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang tắt Bơm 7.5Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức tắt Bơm 7.5Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong tắt Bơm 7.5Kw"));
          }
          kdata24 = true;
        });
      }
      off_Bom3();
    }
  }
}
BLYNK_WRITE(V25)  // Off 18.5 KW
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms0 != 0) && (kdata25)) {
        kdata25 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang tắt Bơm 18.5Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức tắt Bơm 18.5Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong tắt Bơm 18.5Kw"));
          }
          kdata25 = true;
        });
      }
      off_Bom1();
    }
  }
}
BLYNK_WRITE(V26)  // Off G3
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms5 != 0) && (kdata26)) {
        kdata26 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang tắt Giếng 3"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức tắt Giếng 3"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong tắt Giếng 3"));
          }
          kdata26 = true;
        });
      }
      offG3();
    }
  }
}
BLYNK_WRITE(V27)  // On 18.5 KW
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms0 == 0) && (kdata27)) {
        kdata27 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang mở Bơm 18.5Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức mở Bơm 18.5Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong mở Bơm 18.5Kw"));
          }
          kdata27 = true;
        });
      }
      on_Bom1();
    }
  }
}
BLYNK_WRITE(V28)  // On 7.5 KW
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms4 == 0) && (kdata28)) {
        kdata28 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang mở Bơm 7.5Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức mở Bơm 7.5Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong mở Bơm 7.5Kw"));
          }
          kdata28 = true;
        });
      }
      on_Bom3();
    }
  }
}
BLYNK_WRITE(V29)  // On G1
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms3 == 0) && (kdata29)) {
        kdata29 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang mở Giếng 1"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức mở Giếng 1"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong mở Giếng 1"));
          }
          kdata29 = true;
        });
      }
      onG1();
    }
  }
}
BLYNK_WRITE(V30)  // On 11 KW
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms2 == 0) && (kdata30)) {
        kdata30 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang mở Bơm 11Kw"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức mở Bơm 11Kw"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong mở Bơm 11Kw"));
          }
          kdata30 = true;
        });
      }
      on_Bom4();
    }
  }
}
BLYNK_WRITE(V31)  // On G2
{
  if (keySwitchQ || keySwitchP || keySwitchD) {
    if (param.asInt() == HIGH) {
      if ((Irms1 == 0) && (kdata31)) {
        kdata31 = false;
        timer1.setTimeout(3000, []() {
          if (keySwitchQ) {
            Blynk.logEvent("info", String("Quang mở Giếng 2"));
          } else if (keySwitchD) {
            Blynk.logEvent("info", String("Đức mở Giếng 2"));
          } else if (keySwitchP) {
            Blynk.logEvent("info", String("Phong mở Giếng 2"));
          }
          kdata31 = true;
        });
      }
      onG2();
    }
  }
}
BLYNK_WRITE(V47)  // On-Off Nen khi 1
{
  int data47 = param.asInt();
  if (keySwitchQ) {
    if (data47 == 1) {
      pcf8575_1.digitalWrite(pin_NK1, HIGH);
    } else {
      pcf8575_1.digitalWrite(pin_NK1, LOW);
    }
  }
}
BLYNK_WRITE(V48)  // On-Off Nen khi 2
{
  int data48 = param.asInt();
  if (keySwitchQ) {
    if (data48 == 1) {
      pcf8575_1.digitalWrite(pin_NK2, HIGH);
    } else {
      pcf8575_1.digitalWrite(pin_NK2, LOW);
    }
  }
}
BLYNK_WRITE(V58)  // Lưu lượng 1m3 G2
{
  LLG2_1m3 = param.asInt();
}
BLYNK_WRITE(V62)  // Lưu lượng 1m3 G1
{
  LLG1_1m3 = param.asInt();
}
//----------------------------------
BLYNK_WRITE(V10)  // Ap luc
{
  pre = param.asFloat();
  //Blynk.virtualWrite(V5, "ok");
  if (pre < 2.2) {
    n++;
    if ((n >= 5) && (keyPRE2)) {
      if (noti) {
        event_pressure();
      }
      keyPRE2 = false;
      timer1.setTimeout(600000L, []() {  // 15p báo lại
        keyPRE2 = true;
        if (pre >= 2.2) {
          n = 0;
        }
      });
    }
  } else if (pre >= 4.4) {  // Nếu áp lớn hơn 4 bar
    m++;
    if ((m >= 3) && (keyPRE4)) {
      if (noti) {
        event_pressure();
      }
      keyPRE4 = false;
      timer1.setTimeout(180000L, []() {  // 3p báo lại
        keyPRE4 = true;
        if (pre < 4.4) {
          m = 0;
        }
      });
    }
  }
}
//----------------------------------
void readcurrent()  // C3 - 18.5 KW
{
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float rms0 = emon0.calcIrms(740);
  if (rms0 < 3) {
    Irms0 = 0;
    yIrms0 = 0;
  } else if (rms0 > 3) {
    Irms0 = rms0;
    yIrms0 = yIrms0 + 1;
    if ((yIrms0 > 2) && ((Irms0 > data.SetAmpemax) || (Irms0 < data.SetAmpemin))) {
      xSetAmpe = xSetAmpe + 1;
      if ((xSetAmpe >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Máy 18.5KW lỗi: ") + Irms0 + String(" A"));
        pcf8575_1.digitalWrite(pin_off_Bom1, LOW);
        trip0 = true;
        xSetAmpe = 0;
        timer1.setTimeout(600000L, []() {
          pcf8575_1.digitalWrite(pin_off_Bom1, HIGH);
        });
      }
    } else {
      xSetAmpe = 0;
    }
  }
  //Blynk.virtualWrite(V40, Irms0);
}
void readcurrent1()  // C1 - Gieng 2
{
  //Blynk.run();
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float rms1 = emon1.calcIrms(740);
  if (rms1 < 3) {
    Irms1 = 0;
    yIrms1 = 0;
    if ((unsigned long)(millis() - dem2) > 1800000) {
      dem2 = millis();
      event_30p();
    }
  } else if (rms1 > 3) {
    Irms1 = rms1;
    yIrms1 = yIrms1 + 1;
    if (yIrms1 > 3) {
      dem2 = millis();
    }
    if ((yIrms1 > 2) && ((Irms1 > data.SetAmpe1max) || (Irms1 < data.SetAmpe1min))) {
      xSetAmpe1 = xSetAmpe1 + 1;
      if ((xSetAmpe1 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Giếng II lỗi: ") + Irms1 + String(" A"));
        trip1 = true;
        xSetAmpe1 = 0;
        pcf8575_1.digitalWrite(pin_off_G2, LOW);
        timer.setTimeout(600000L, []() {
          pcf8575_1.digitalWrite(pin_off_G2, HIGH);
        });
      }
    } else {
      xSetAmpe1 = 0;
    }
  }
  //Blynk.virtualWrite(V41, Irms1);
}
void readcurrent2()  // C6 - 11 KW
{
  //Blynk.run();
  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, LOW);
  float rms2 = emon2.calcIrms(740);
  if (rms2 < 3) {
    Irms2 = 0;
    yIrms2 = 0;
  } else if (rms2 > 3) {
    Irms2 = rms2;
    yIrms2 = yIrms2 + 1;
    if ((yIrms2 > 2) && ((Irms2 > data.SetAmpe2max) || (Irms2 < data.SetAmpe2min))) {
      xSetAmpe2 = xSetAmpe2 + 1;
      if ((xSetAmpe2 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Máy 11KW lỗi: ") + Irms2 + String(" A"));
        trip2 = true;
        xSetAmpe2 = 0;
        pcf8575_1.digitalWrite(pin_off_Bom4, LOW);
        timer.setTimeout(600000L, []() {
          pcf8575_1.digitalWrite(pin_off_Bom4, HIGH);
        });
      }
    } else {
      xSetAmpe2 = 0;
    }
  }
  //Blynk.virtualWrite(V42, Irms2);  // Irms2 - 11
}
void readcurrent3()  // C0 - Gieng 1
{
  //Blynk.run();
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float rms3 = emon3.calcIrms(740);
  if (rms3 < 3) {
    Irms3 = 0;
    yIrms3 = 0;
    if ((unsigned long)(millis() - dem1) > 1800000) {
      dem1 = millis();
      event_30p();
    }
  } else if (rms3 > 3) {
    Irms3 = rms3;
    yIrms3 = yIrms3 + 1;
    if (yIrms3 > 3) {
      dem1 = millis();
    }
    if ((yIrms3 > 2) && ((Irms3 > data.SetAmpe3max) || (Irms3 < data.SetAmpe3min))) {
      xSetAmpe3 = xSetAmpe3 + 1;
      if ((xSetAmpe3 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Giếng I lỗi: ") + Irms3 + String(" A"));
        trip3 = true;
        xSetAmpe3 = 0;
        pcf8575_1.digitalWrite(pin_off_G1, LOW);
        timer.setTimeout(600000L, []() {
          pcf8575_1.digitalWrite(pin_off_G1, HIGH);
        });
      }
    } else {
      xSetAmpe3 = 0;
    }
  }
  //Blynk.virtualWrite(V43, Irms3);  // Irms3 - G1
}
void readcurrent4()  // C5 - 7.5 KW
{
  //Blynk.run();
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, LOW);
  float rms4 = emon4.calcIrms(740);
  if (rms4 < 3) {
    Irms4 = 0;
    yIrms4 = 0;
  } else if (rms4 > 3) {
    Irms4 = rms4;
    yIrms4 = yIrms4 + 1;
    if ((yIrms4 > 2) && ((Irms4 > data.SetAmpe4max) || (Irms4 < data.SetAmpe4min))) {
      xSetAmpe4 = xSetAmpe4 + 1;
      if ((xSetAmpe4 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Máy 7.5KW lỗi: ") + Irms4 + String(" A"));
        trip4 = true;
        xSetAmpe4 = 0;
        pcf8575_1.digitalWrite(pin_off_Bom3, LOW);
        timer.setTimeout(300000L, []() {
          pcf8575_1.digitalWrite(pin_off_Bom3, HIGH);
        });
      }
    } else {
      xSetAmpe4 = 0;
    }
  }
  //Blynk.virtualWrite(V44, Irms4);  // Irms4 - 7.5
}
void readcurrent5()  // C2 - Gieng 3
{
  //Blynk.run();
  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  float rms5 = emon5.calcIrms(740);
  if (rms5 < 3) {
    Irms5 = 0;
    yIrms5 = 0;
    if ((unsigned long)(millis() - dem3) > 1800000) {
      dem3 = millis();
      //event_30p();
    }
  } else if (rms5 >= 3) {
    Irms5 = rms5;
    yIrms5 = yIrms5 + 1;
    if (yIrms5 > 3) {
      dem3 = millis();
    }
    if ((yIrms5 > 2) && ((Irms5 > data.SetAmpe5max) || (Irms5 < data.SetAmpe5min))) {
      xSetAmpe5 = xSetAmpe5 + 1;
      if ((xSetAmpe5 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Giếng III lỗi: ") + Irms5 + String(" A"));
        trip5 = true;
        xSetAmpe5 = 0;
        pcf8575_1.digitalWrite(pin_off_G3, LOW);
        timer.setTimeout(300000L, []() {
          pcf8575_1.digitalWrite(pin_off_G3, HIGH);
        });
      }
    } else {
      xSetAmpe5 = 0;
    }
  }
  //Blynk.virtualWrite(V45, Irms5);  // Irms5 - G3
}
void readcurrent6()  // C4 - 30kw
{
  //Blynk.run();
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, LOW);
  float rms6 = emon6.calcIrms(740);
  if (rms6 < 3) {
    Irms6 = 0;
    yIrms6 = 0;
  } else if (rms6 >= 3) {
    Irms6 = rms6;
    yIrms6 = yIrms6 + 1;
    if ((yIrms6 > 2) && ((Irms6 >= data.SetAmpe6max) || (Irms6 < data.SetAmpe6min))) {
      xSetAmpe6 = xSetAmpe6 + 1;
      if ((xSetAmpe6 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Máy 30KW lỗi: ") + Irms6 + String(" A"));
        trip6 = true;
        xSetAmpe6 = 0;
        pcf8575_1.digitalWrite(pin_off_Bom2, LOW);
        timer.setTimeout(3000L, []() {
          pcf8575_1.digitalWrite(pin_off_Bom2, HIGH);
        });
      }
    } else {
      xSetAmpe6 = 0;
    }
  }
  //Blynk.virtualWrite(V46, Irms6);  // Irms6 - 30kw
}
void readcurrent7()  // C7 - NK1
{
  //Blynk.run();
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, LOW);
  float rms7 = emon7.calcIrms(740);
  if (rms7 < 3) {
    Irms7 = 0;
    yIrms7 = 0;
  } else if (rms7 >= 3) {
    Irms7 = rms7;
    yIrms7 = yIrms7 + 1;
    if ((yIrms7 > 2) && ((Irms7 >= data.SetAmpe7max) || (Irms7 < data.SetAmpe7min))) {
      xSetAmpe7 = xSetAmpe7 + 1;
      if ((xSetAmpe7 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Máy NÉN KHÍ 1 lỗi: ") + Irms7 + String(" A"));
        trip7 = true;
        xSetAmpe7 = 0;
        pcf8575_1.digitalWrite(pin_NK1, HIGH);
      }
    } else {
      xSetAmpe7 = 0;
    }
  }
  //Blynk.virtualWrite(V32, Irms7);  // Irms7 - NK1
}
void readcurrent8()  // C8 - NK2
{
  //Blynk.run();
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  float rms8 = emon8.calcIrms(740);
  if (rms8 < 1) {
    Irms8 = 0;
    yIrms8 = 0;
  } else if (rms8 >= 1) {
    Irms8 = rms8;
    yIrms8 = yIrms8 + 1;
    if ((yIrms8 > 2) && ((Irms8 >= data.SetAmpe8max) || (Irms8 < data.SetAmpe8min))) {
      xSetAmpe8 = xSetAmpe8 + 1;
      if ((xSetAmpe8 >= 3) && (data.protect)) {
        Blynk.logEvent("error", String("Máy NÉN KHÍ 2 lỗi: ") + Irms8 + String(" A"));
        trip8 = true;
        xSetAmpe8 = 0;
        pcf8575_1.digitalWrite(pin_NK2, HIGH);
      }
    } else {
      xSetAmpe8 = 0;
    }
  }
  //Blynk.virtualWrite(V36, Irms8);  // Irms6 - 30kw
}
void rtctime()  // Irms0 :18.5Kw    Imrs2:11Kw    Imrs4:7.5Kw
{
  if (data.status_rualoc != 0) {
    bridge_Tram2C(Tram2_Rualoc, 0, data.status_rualoc);
  }
  //---------------------------------
  DateTime now = rtc_module.now();
  if (blynk_first_connect == true) {
    if ((now.day() != day()) || (now.hour() != hour()) || ((now.minute() - minute() > 2) || (minute() - now.minute() > 2))) {
      rtc_module.adjust(DateTime(year(), month(), day(), hour(), minute(), second()));
      DateTime now = rtc_module.now();
    }
  }
  timestamp = now.unixtime();
  Blynk.virtualWrite(V1, daysOfTheWeek[now.dayOfTheWeek()], ", ", now.day(), "/", now.month(), "/", now.year(), " - ", now.hour(), ":", now.minute(), ":", now.second());
  /*
  startt = data.startH * 36 + data.startM * 0.6;
  stopt = data.stopH * 36 + data.stopM * 0.6;
  float nowtime = (now.hour() * 36 + now.minute() * 0.6);
  if (startt > stopt) {                             // Nếu giờ chạy lớn hơn giờ tắt
    if ((nowtime < startt) && (nowtime > stopt)) {  // Bắt đầu thời gian nghỉ
      noti = false;
      if ((Irms6 != 0)) {  // off máy 30
        off_Bom2();
      }
      if ((Irms0 != 0)) {  // off máy 18.5
        off_Bom1();
      }
      if (Irms2 != 0) {  // off máy 11
        off_Bom4();
      }
      if (Irms4 != 0) {  // off máy 7.5
        off_Bom3();
      }
    }
  }
  if (startt < stopt) {                               // Nếu giờ chạy nhỏ hơn giờ tắt
    if (((nowtime < startt) || (nowtime > stopt))) {  // Bắt đầu thời gian nghỉ
      noti = false;
      if ((Irms6 != 0)) {  // off máy 30
        off_Bom2();
      }
      if ((Irms0 != 0)) {  // off máy 18.5
        off_Bom1();
      }
      if (Irms2 != 0) {  // off máy 11
        off_Bom4();
      }
      if (Irms4 != 0) {  // off máy 7.5
        off_Bom3();
      }
    }
  }
  if ((nowtime > startt) && (nowtime < (startt + 18))) {  // Hết thời gian nghỉ + 30p
    if ((Irms0 == 0) && (Irms2 == 0) && (Irms4 == 0) && (Irms6 == 0)) {
      timer.setTimeout(1800000L, []() {  // Sau 30 kể từ lúc chạy máy
        noti = true;
      });
      if (trip2 == false) {  // Nếu máy 11kw không lỗi
        on_Bom4();           // Chạy máy 11KW
      }
    }
  }
  */
  timer.restartTimer(timer_I);
}
//----------------------------------------------------
void MeasureCmForSmoothing()  //C14
{
  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  float sensorValue = analogRead(A0);
  distance1 = (((sensorValue - zeropointTank) * 800) / (fullpointTank - zeropointTank));  // 915,74 (R=147.7)
  //Serial.print("sensorValue ");
  //Serial.println(distance1);
  if (distance1 > 0) {
    smoothDistance = digitalSmooth(distance1, sensSmoothArray1);
    volume1 = (dai * smoothDistance * rong) / 1000000;
    //Serial.print("\nsmoothDistance ");
    //Serial.println(smoothDistance);
  }
  if (smoothDistance >= 500)  //  Nếu mực nước = 500cm thì tắt Cấp 1
  {
    if (data.protect) {
      if (Irms1 != 0) {
        offG2();
      }
      if (Irms3 != 0) {
        offG1();
      }
      if (Irms5 != 0) {
        offG3();
      }
    }
  }
}
//----------------------------------------------------
BLYNK_WRITE(V50) {
  String dataS = param.asStr();
  if ((dataS == "ok") || (dataS == "Ok") || (dataS == "OK") || (dataS == "oK")) {
    if (clo_cache > 0) {
      data.clo = clo_cache;
      clo_cache = 0;
      data.time_clo = timestamp;
      Blynk.virtualWrite(V56, data.clo);
      savedata();
      terminal_luuluong.clear();
      Blynk.virtualWrite(V50, "Đã lưu - CLO:", data.clo, "kg");
    }
  }
}
BLYNK_WRITE(V51) {
  if (param.asFloat() > 0) {
    terminal_luuluong.clear();
    clo_cache = param.asFloat();
    Blynk.virtualWrite(V50, " Lượng CLO châm hôm nay:", clo_cache, "kg\n Vui lòng kiểm tra kỹ, nếu đúng hãy nhập 'OK' để lưu");
  }
}
BLYNK_WRITE(V52) {
  if (param.asInt() == 1) {
    DateTime dt(data.time_clo);
    terminal_luuluong.clear();
    Blynk.virtualWrite(V50, "Châm CLO:", data.clo, "kg vào lúc", dt.hour(), ":", dt.minute(), "-", dt.day(), "/", dt.month(), "/", dt.year());
  }
}
//----------------------------------------------------
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
}  //-------------------------
void updata() {
  String server_path = server_name + "batch/update?token=" + BLYNK_AUTH_TOKEN
                       + "&V32=" + Irms7
                       + "&V34=" + smoothDistance
                       + "&V35=" + volume1
                       + "&V36=" + Irms8
                       + "&V40=" + Irms0
                       + "&V41=" + Irms1
                       + "&V42=" + Irms2
                       + "&V43=" + Irms3
                       + "&V44=" + Irms4
                       + "&V45=" + Irms5
                       + "&V46=" + Irms6;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  http.end();
}
//----------------------------------------------------
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(5000);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  emon0.current(A0, 110);
  emon1.current(A0, 112);
  emon2.current(A0, 112);
  emon3.current(A0, 110);
  emon4.current(A0, 112);
  emon5.current(A0, 110);
  emon6.current(A0, 109);
  emon7.current(A0, 109);
  emon8.current(A0, 109);

  pcf8575_1.pinMode(pin_on_G1, OUTPUT);
  pcf8575_1.digitalWrite(pin_on_G1, HIGH);
  pcf8575_1.pinMode(pin_off_G1, OUTPUT);
  pcf8575_1.digitalWrite(pin_off_G1, HIGH);
  pcf8575_1.pinMode(pin_on_G2, OUTPUT);
  pcf8575_1.digitalWrite(pin_on_G2, HIGH);
  pcf8575_1.pinMode(pin_off_G2, OUTPUT);
  pcf8575_1.digitalWrite(pin_off_G2, HIGH);
  pcf8575_1.pinMode(pin_on_G3, OUTPUT);
  pcf8575_1.digitalWrite(pin_on_G3, HIGH);
  pcf8575_1.pinMode(pin_off_G3, OUTPUT);
  pcf8575_1.digitalWrite(pin_off_G3, HIGH);

  pcf8575_1.pinMode(pin_on_Bom1, OUTPUT);
  pcf8575_1.digitalWrite(pin_on_Bom1, HIGH);
  pcf8575_1.pinMode(pin_off_Bom1, OUTPUT);
  pcf8575_1.digitalWrite(pin_off_Bom1, HIGH);
  pcf8575_1.pinMode(pin_on_Bom2, OUTPUT);
  pcf8575_1.digitalWrite(pin_on_Bom2, HIGH);
  pcf8575_1.pinMode(pin_off_Bom2, OUTPUT);
  pcf8575_1.digitalWrite(pin_off_Bom2, HIGH);
  pcf8575_1.pinMode(pin_on_Bom3, OUTPUT);
  pcf8575_1.digitalWrite(pin_on_Bom3, HIGH);
  pcf8575_1.pinMode(pin_off_Bom3, OUTPUT);
  pcf8575_1.digitalWrite(pin_off_Bom3, HIGH);
  pcf8575_1.pinMode(pin_on_Bom4, OUTPUT);
  pcf8575_1.digitalWrite(pin_on_Bom4, HIGH);
  pcf8575_1.pinMode(pin_off_Bom4, OUTPUT);
  pcf8575_1.digitalWrite(pin_off_Bom4, HIGH);

  pcf8575_1.pinMode(pin_NK1, OUTPUT);
  pcf8575_1.digitalWrite(pin_NK1, HIGH);
  pcf8575_1.pinMode(pin_NK2, OUTPUT);
  pcf8575_1.digitalWrite(pin_NK2, HIGH);

  rtc_module.begin();

  eeprom.initialize();
  eeprom.readBytes(address, sizeof(dataDefault), (byte*)&data);

  timer.setTimeout(5000L, []() {
    timer_I = timer.setInterval(1589, []() {
      readcurrent();
      readcurrent1();
      readcurrent2();
      readcurrent3();
      readcurrent4();
      readcurrent5();
      readcurrent6();
      readcurrent7();
      readcurrent8();
      updata();
      timer.restartTimer(timer_I);
      timer.restartTimer(timer_tank);
    });
    timer_tank = timer.setInterval(230L, MeasureCmForSmoothing);
    timer.setInterval(15005L, []() {
      rtctime();
      timer.restartTimer(timer_I);
      timer.restartTimer(timer_tank);
    });
    timer.setInterval(900005L, []() {
      connectionstatus();
      timer.restartTimer(timer_I);
      timer.restartTimer(timer_tank);
    });
  });
  keyterminal.clear();
}

void loop() {
  Blynk.run();
  timer.run();
  timer1.run();
}