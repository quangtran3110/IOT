
#define BLYNK_TEMPLATE_ID "TMPL0DBjAEt-"
#define BLYNK_TEMPLATE_NAME "VOLUME"
#define BLYNK_AUTH_TOKEN "XBbjtsi_sGTyvvs_zF4yN2s_4OoRvMjA"
#define BLYNK_FIRMWARE_VERSION "231220.T2.VL.G2"
#define BLYNK_PRINT Serial
#define APP_DEBUG

const char* ssid = "Hiddennet";
const char* password = "Password";
//const char* ssid = "Wifi";
//const char* password = "Password";

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#include <WidgetRTC.h>
#include "RTClib.h"

#include <I2C_eeprom.h>
#include <I2C_eeprom_cyclic_store.h>
#define MEMORY_SIZE 0x4000  // Total capacity of the EEPROM
#define PAGE_SIZE 64

#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_name = "http://sgp1.blynk.cloud/external/api/";
String Main = "o-H-k28kNBIzgNIAP89f2AElv--eWuVO";
#define URL_fw_Bin "https://github.com/quangtran3110/IOT/raw/main/Arduino/Tram2/volumeG2/build/esp8266.esp8266.nodemcuv2/volumeG2.ino.bin"

bool blynk_first_connect = false;
int var_10m3;
byte reboot_num;

struct Data {
public:
  uint32_t pulse, save_num;
  byte save_day;
};
Data data, dataCheck;
const struct Data dataDefault = { 0, 0, 0 };


I2C_eeprom ee(0x50, MEMORY_SIZE);
I2C_eeprom_cyclic_store<Data> cs;

WidgetTerminal terminal(V0);
WidgetRTC rtc_widget;
BlynkTimer timer;
BLYNK_CONNECTED() {
  rtc_widget.begin();
  blynk_first_connect = true;
  Blynk.setProperty(V0, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
}

ICACHE_RAM_ATTR void buttonPressed() {
  data.pulse++;
}

void savedata() {
  if (memcmp(&data, &dataCheck, sizeof(dataDefault)) == 0) {
    // Serial.println("structures same no need to write to EEPROM");
  } else {
    // Serial.println("\nWrite bytes to EEPROM memory...");
    data.save_num = data.save_num + 1;
    cs.write(data);
  }
  Blynk.setProperty(V0, "label", BLYNK_FIRMWARE_VERSION, "-EEPROM ", data.save_num);
}
void send_LL_24h() {
  String server_path = server_name + "batch/update?token=" + Main
                       + "&V59=" + data.pulse;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
  }
  http.end();
}
void send_LL_1m3() {
  String server_path = server_name + "batch/update?token=" + Main
                       + "&V60=1"
                       + "&V58=" + data.pulse;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
  }
  http.end();
}
BLYNK_WRITE(V0) {
  String dataS = param.asStr();
  if (dataS == "save_num") {
    terminal.clear();
    Blynk.virtualWrite(V0, "Số lần ghi EEPROM: ", data.save_num);
  } else if (dataS == "rst") {
    terminal.clear();
    Blynk.virtualWrite(V0, "ESP khởi động lại sau 3s");
    delay(3000);
    ESP.restart();
  } else if (dataS == "update") {
    terminal.clear();
    Blynk.virtualWrite(V0, "UPDATE FIRMWARE...");
    update_fw();
  } else if (dataS == "reset_vl") {
    terminal.clear();
    Blynk.virtualWrite(V0, "Volume RESET...");
    data.pulse = 0;
    savedata();
    Blynk.virtualWrite(V0, "\nSản lượng hôm nay là:", data.pulse, "m3");
  } else if (dataS == "savedata") {
    terminal.clear();
    savedata();
    Blynk.virtualWrite(V0, "DATA_SAVE... ok!");
  } else {
    Blynk.virtualWrite(V0, "Mật mã sai.\nVui lòng nhập lại!\n");
  }
}

void rtc_time() {
  send_LL_1m3();
  if (blynk_first_connect) {
    //Serial.println(data.save_day);
    if (data.save_day != day()) {
      if (Blynk.connected()) {
        send_LL_24h();
        data.pulse = 0;
        data.save_day = day();
        savedata();
      }
    }
  }
  if (((data.pulse % 10) == 0) && (data.pulse != var_10m3)) {
    var_10m3 = data.pulse;
    savedata();
  }
}
//-------------------------
void connectionstatus() {
  if ((WiFi.status() != WL_CONNECTED)) {
    //Serial.println("Khong ket noi WIFI");
  }
  if ((WiFi.status() == WL_CONNECTED) && (!Blynk.connected())) {
    reboot_num = reboot_num + 1;
    if ((reboot_num == 1) || (reboot_num == 2)) {
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
//-------------------------
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(5000);

  ee.begin();
  cs.begin(ee, PAGE_SIZE, MEMORY_SIZE / PAGE_SIZE);
  cs.read(data);

  attachInterrupt(D6, buttonPressed, RISING);

  timer.setInterval(15003, rtc_time);
}

void loop() {
  Blynk.run();
  timer.run();
}
