#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLFpa9NbqF"
#define BLYNK_DEVICE_NAME "Tân Lập"

#define I2C_ADDRESS 0x40
#define APP_DEBUG

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "INA226_WE.h"
#include <EEPROM.h>

INA226_WE ina226 = INA226_WE(I2C_ADDRESS);

char auth[] = "0mWyl3eT8xTCqSlVecGw6DzzNPhTadoa";
const char* ssid = "net";
const char* password = "Password";

float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0;
float Result1;
float value14 = 0;
byte reboot_num;
int save_num;
int time1, time2, time3;
BlynkTimer timer;
BLYNK_CONNECTED() {
}
void savedata() {
  save_num = save_num + 1;
  EEPROM.begin(512);
  delay(20);
  EEPROM.put(160, reboot_num);
  EEPROM.put(162, save_num);
  EEPROM.commit();
  EEPROM.end();
  Blynk.virtualWrite(V35, save_num);
}

void monitor() {
  ina226.readAndClearFlags();
  shuntVoltage_mV = ina226.getShuntVoltage_mV();
  busVoltage_V = ina226.getBusVoltage_V();
  current_mA = ina226.getCurrent_mA();
  power_mW = ina226.getBusPower();
  loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);

  Blynk.virtualWrite(V30, busVoltage_V);
  Blynk.virtualWrite(V31, current_mA / 1000);
  Blynk.virtualWrite(V32, power_mW / 1000);
}

void pressure() {
  float sensorValue = analogRead(A0);
  float Result;
  Result = (((sensorValue - 195) * 5) / (950 - 195));

  //Serial.println(sensorValue);
  //Serial.println(Result);
  if (Result > 0) {
    value14 += Result;
    Result1 = value14 / 16.0;
    value14 -= Result1;
  }
}

void updata() {
  Blynk.virtualWrite(V33, Result1);
  Blynk.virtualWrite(V34, digitalRead(D1));
}

void connectionstatus() {
  if ((WiFi.status() != WL_CONNECTED)) {
    //Serial.println("Khong ket noi WIFI");
  }
  if ((WiFi.status() == WL_CONNECTED) && (!Blynk.connected())) {
    reboot_num = reboot_num + 1;
    savedata();
    //Serial.print("reboot_num: ");
    //Serial.println(reboot_num);
    if (reboot_num == 1) {
      //Serial.println("Restart...");
      //Serial.print("reboot_num: ");
      //Serial.println(reboot_num);
      delay(1000);
      ESP.restart();
    }
    if (reboot_num % 5 == 0) {
      //Serial.print("reboot_num: ");
      //Serial.println(reboot_num);
      delay(1000);
      ESP.restart();
    }
  }
  if (Blynk.connected()) {
    if (reboot_num != 0) {
      reboot_num = 0;
      savedata();
    }
    //Serial.print("reboot_num: ");
    //Serial.println(reboot_num);
    //Serial.println("Blynk OK");
  }
}


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(auth);
  delay(5000);

  EEPROM.begin(512);
  delay(10);
  EEPROM.begin(512);
  delay(20);
  EEPROM.get(160, reboot_num);
  EEPROM.get(162, save_num);
  EEPROM.commit();
  EEPROM.end();

  Wire.begin();
  ina226.init();
  ina226.setAverage(AVERAGE_16);             // choose mode and uncomment for change of default
  ina226.setConversionTime(CONV_TIME_1100);  //choose conversion time and uncomment for change of default
  ina226.setMeasureMode(CONTINUOUS);         // choose mode and uncomment for change of default
  ina226.setCurrentRange(MA_800);            // choose gain and uncomment for change of default
  ina226.waitUntilConversionCompleted();     //if you comment this line the first data might be zero
  delay(1000);

  time1 = timer.setInterval(2923, monitor);
  time2 = timer.setInterval(213, pressure);
  time3 = timer.setInterval(1301, []() {
    updata();
    timer.restartTimer(time2);
  });
  timer.setInterval(420005L, []() { //7p
    connectionstatus();
    timer.restartTimer(time1);
    timer.restartTimer(time2);
    timer.restartTimer(time3);
  });
}
void loop() {
  Blynk.run();
  timer.run();
}