#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "INA226_WE.h"
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define nameesp "[HOME.IROBOT]"
OneWire oneWire(D7);
DallasTemperature sensors(&oneWire);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "2.asia.pool.ntp.org", 3600 * 7, 60000);
INA226_WE ina226 = INA226_WE(0x40);

char tz[] = "Asia/Ho_Chi_Minh";
const int pin_run = D6;
const int pin_dock = D5;

char auth[] = "L9MuvVtLD5XFsHjds9dE5L8GFmOCoSUR";
const char* ssid = "Wifi";
const char* password = "Password";
//const char* ssid = "tram bom so 4";
//const char* password = "0943950555";

float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0;
float temp[1];
int second_, minute_, hour_, day_, month_, year_;
int rowIndex = 0;
int startH, startM, stopH, stopM, startt, stopt, nowtime;
int l1, l2;
byte r;

String time1, time2;

WidgetBridge CHARGE(V0);

WidgetTable table;
BLYNK_ATTACH_WIDGET(table, V10);
BlynkTimer timer;
BLYNK_CONNECTED() {
  CHARGE.setAuthToken("L9MuvVtLD5XFsHjds9dE5L8GFmOCoSUR");
  table.clear();
  table.addRow(0, "Điện áp", 0);
  table.addRow(1, "Dòng điện", 0);
  table.addRow(2, "Công suất", 0);
  table.addRow(3, "Nhiệt độ pin", 0);
  table.addRow(4, "Chạy lần 1", time1);
  table.addRow(5, "Chạy lần 2", time2);
  if (l1 == 0) {
    Blynk.virtualWrite(V10, "deselect", 4);
  } else Blynk.virtualWrite(V10, "select", 4);
  if (l2 == 0) {
    Blynk.virtualWrite(V10, "deselect", 5);
  } else Blynk.virtualWrite(V10, "select", 5);
}

void tem() {
  sensors.requestTemperatures();
  //Serial.println(sensors.getDeviceCount());
  for (byte i = 0; i < sensors.getDeviceCount(); i++) {
    temp[i] = sensors.getTempCByIndex(i);
    //Serial.println(temp[i]);
  }
  if (temp[0] > 50) {
    Blynk.notify("Nhiệt độ pin IROBOT quá nóng!!!");
    CHARGE.virtualWrite(V5, 0);
    if (current_mA >= 600) {
      digitalWrite(pin_run, LOW);
      delay(3000);
      digitalWrite(pin_run, HIGH);
    }
  }
  Blynk.virtualWrite(V8, temp[0]);
  String nhiet_do = String(temp[0]) + " °C";
  Blynk.virtualWrite(V10, "update", 3, "Nhiệt độ pin", nhiet_do);
}

void monitor() {
  ina226.readAndClearFlags();
  shuntVoltage_mV = ina226.getShuntVoltage_mV();
  busVoltage_V = ina226.getBusVoltage_V();
  current_mA = ina226.getCurrent_mA();
  power_mW = ina226.getBusPower();
  loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);

  if (current_mA >= 1100) {
    Blynk.notify("Dòng tiêu thụ IROBOT quá cao!!!");
    digitalWrite(pin_run, LOW);
    delay(3000);
    digitalWrite(pin_run, HIGH);
  }
  Blynk.virtualWrite(V1, busVoltage_V);
  Blynk.virtualWrite(V2, current_mA);
  Blynk.virtualWrite(V3, power_mW / 1000);

  String dien_ap = String(busVoltage_V) + " V";
  String dong_dien = String(current_mA) + " mA";
  String cong_suat = String(power_mW / 1000) + " W";

  Blynk.virtualWrite(V10, "update", 0, "Điện áp", dien_ap);
  Blynk.virtualWrite(V10, "update", 1, "Dòng điện", dong_dien);
  Blynk.virtualWrite(V10, "update", 2, "Công suất", cong_suat);
}

void gettime() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);
  day_ = ptm->tm_mday;
  month_ = ptm->tm_mon + 1;
  year_ = ptm->tm_year + 1900;
  second_ = timeClient.getSeconds();
  minute_ = timeClient.getMinutes();
  hour_ = timeClient.getHours();
  nowtime = hour_ * 3600 + minute_ * 60;
  //String rtc_time = String(day_) + "/" + String(month_) + "/" + String(year_) + "  " + String(hour_) + ":" + String(minute_);
  //Blynk.virtualWrite(V13, rtc_time);
  Serial.println(nowtime);
  Serial.println(l1);
  Serial.println(l2);
  Serial.println("...");
  if (((nowtime == l1 || (((nowtime - l1) > 0) && ((nowtime - l1) < 300))) || (nowtime == l2 || (((nowtime - l2) > 0) && ((nowtime - l2) < 300)))) && (busVoltage_V >= 16) && (current_mA < 600)) {
    CHARGE.virtualWrite(V5, 0);
    timer.setTimeout(1000, []() {
      digitalWrite(pin_run, LOW);
      delay(100);
      digitalWrite(pin_run, HIGH);
    });
  }
}

BLYNK_WRITE(V4) {  //time input
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    startH = t.getStartHour();
    startM = t.getStartMinute();
    startt = startH * 3600 + startM * 60;
    if (r == 1) {
      l1 = startt;
      EEPROM.begin(512);
      delay(20);
      EEPROM.put(160, l1);
      EEPROM.commit();
      EEPROM.end();
      delay(100);
      time1 = String(int(l1 / 3600)) + " : " + String(int(l1 / 60) - (int(l1 / 3600) * 60));
      Blynk.virtualWrite(V10, "update", 4, "Chạy lần 1", time1);
      if (l1 == 0) {
        Blynk.virtualWrite(V10, "deselect", 4);
      } else Blynk.virtualWrite(V10, "select", 4);
    } else if (r == 2) {
      l2 = startt;
      EEPROM.begin(512);
      delay(20);
      EEPROM.put(164, l2);
      EEPROM.commit();
      EEPROM.end();
      delay(100);
      time2 = String(int(l2 / 3600)) + " : " + String(int(l2 / 60) - (int(l2 / 3600) * 60));
      Blynk.virtualWrite(V10, "update", 5, "Chạy lần 2", time2);
      if (l2 == 0) {
        Blynk.virtualWrite(V10, "deselect", 5);
      } else Blynk.virtualWrite(V10, "select", 5);
    }
  }
}
BLYNK_WRITE(V6) {  // Chon lần chạy
  switch (param.asInt()) {
    case 1:  // Lan 1
      r = 1;
      Blynk.virtualWrite(V4, l1, 0, tz);
      break;
    case 2:  // Lan 2
      r = 2;
      Blynk.virtualWrite(V4, l2, 0, tz);
      break;
  }
}
BLYNK_WRITE(V7) {  // Save giờ chạy
  if (param.asInt() == 1) {
    if (r == 1) {
      l1 = startt;
      EEPROM.begin(512);
      delay(20);
      EEPROM.put(160, l1);
      EEPROM.commit();
      EEPROM.end();
      delay(100);
      time1 = String(int(l1 / 3600)) + " : " + String(int(l1 / 60) - (int(l1 / 3600) * 60));
      Blynk.virtualWrite(V10, "update", 4, "Chạy lần 1", time1);
      if (l1 == 0) {
        Blynk.virtualWrite(V10, "deselect", 4);
      } else Blynk.virtualWrite(V10, "select", 4);
    } else if (r == 2) {
      l2 = startt;
      EEPROM.begin(512);
      delay(20);
      EEPROM.put(164, l2);
      EEPROM.commit();
      EEPROM.end();
      delay(100);
      time2 = String(int(l2 / 3600)) + " : " + String(int(l2 / 60) - (int(l2 / 3600) * 60));
      Blynk.virtualWrite(V10, "update", 5, "Chạy lần 2", time2);
      if (l2 == 0) {
        Blynk.virtualWrite(V10, "deselect", 5);
      } else Blynk.virtualWrite(V10, "select", 5);
    }
  }
}
BLYNK_WRITE(V9) {
  if (param.asInt() == 1) {
    CHARGE.virtualWrite(V5, 0);
    timer.setTimeout(1000, []() {
      if (current_mA < 100) {
        digitalWrite(pin_run, LOW);
        delay(100);
        digitalWrite(pin_run, HIGH);
        delay(300);
        digitalWrite(pin_run, LOW);
        delay(100);
        digitalWrite(pin_run, HIGH);
      }
      if (current_mA >= 100) {
        digitalWrite(pin_run, LOW);
        delay(100);
        digitalWrite(pin_run, HIGH);
      }
    });
  }
}
BLYNK_WRITE(V11) {
  if (param.asInt() == 1) {
    CHARGE.virtualWrite(V5, 1);
    timer.setTimeout(1000, []() {
      if (current_mA < 100) {
        digitalWrite(pin_dock, LOW);
        delay(100);
        digitalWrite(pin_dock, HIGH);
        delay(300);
        digitalWrite(pin_dock, LOW);
        delay(100);
        digitalWrite(pin_dock, HIGH);
      }
      if (current_mA >= 100) {
        digitalWrite(pin_dock, LOW);
        delay(100);
        digitalWrite(pin_dock, HIGH);
      }
    });
  }
}
BLYNK_WRITE(V12) {
  if (param.asInt() == 1) {
    digitalWrite(pin_run, LOW);
    delay(3000);
    digitalWrite(pin_run, HIGH);
  }
}
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(auth, "Blynkkwaco.ddns.net", 8080);
  delay(5000);
  //-------------------------------
  pinMode(pin_run, OUTPUT);
  digitalWrite(pin_run, HIGH);
  pinMode(pin_dock, OUTPUT);
  digitalWrite(pin_dock, HIGH);

  timeClient.begin();
  EEPROM.begin(512);
  delay(10);
  EEPROM.begin(512);
  delay(20);
  EEPROM.get(160, l1);
  EEPROM.get(164, l2);
  EEPROM.commit();
  EEPROM.end();
  time1 = String(int(l1 / 3600)) + " : " + String(int(l1 / 60) - (int(l1 / 3600) * 60));
  time2 = String(int(l2 / 3600)) + " : " + String(int(l2 / 60) - (int(l2 / 3600) * 60));

  Wire.begin();
  sensors.begin();

  ina226.init();
  ina226.setAverage(AVERAGE_16);             // choose mode and uncomment for change of default
  ina226.setConversionTime(CONV_TIME_1100);  //choose conversion time and uncomment for change of default
  ina226.setMeasureMode(CONTINUOUS);         // choose mode and uncomment for change of default
  ina226.setCurrentRange(MA_800);            // choose gain and uncomment for change of default
  //Serial.println("INA226 Current Sensor Example Sketch - Continuous");
  ina226.waitUntilConversionCompleted();  //if you comment this line the first data might be zero
  delay(1000);

  table.onOrderChange([](int indexFrom, int indexTo) {
  });

  table.onSelectChange([](int index, bool selected) {
  });

  timer.setInterval(7003, gettime);
  timer.setInterval(1203, monitor);
  timer.setInterval(3031, tem);
}

void loop() {
  timer.run();
  Blynk.run();
}
