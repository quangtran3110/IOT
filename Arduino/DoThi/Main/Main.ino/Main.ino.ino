#define BLYNK_TEMPLATE_ID "TMPL6LdmF6nY7"
#define BLYNK_TEMPLATE_NAME "Đô Thị"
#define BLYNK_AUTH_TOKEN "Oyy7F8HDxVurrNg0QOSS6gjsCSQTsDqZ"
#define BLYNK_FIRMWARE_VERSION "240106"

#define caucuadong_TOKEN "T_t2Zf5BO_bfdr4CrPaY5sbQ-BsImZ0w"

#define BLYNK_PRINT Serial
#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
//const char* ssid = "wifi";
//const char* password = "Password";
const char* ssid = "tram bom so 4";
const char* password = "0943950555";

#include <WidgetRTC.h>
char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
char tz[] = "Asia/Ho_Chi_Minh";

#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/IOT/main/Arduino/Tram2BPT/main/build/esp8266.esp8266.nodemcuv2/main.ino.bin"
String main_sever = "http://sgp1.blynk.cloud/external/api/";
String salve_sever = "http://fra1.blynk.cloud/external/api/";

int khu_vuc = 0, dia_diem = 0, van = 0;
uint32_t start_, stop_;
byte reboot_num;
bool blynk_first_connect = false, key_set = false;

WidgetRTC rtc_widget;
WidgetTerminal terminal(V0);

BlynkTimer timer;
BLYNK_CONNECTED() {
  rtc_widget.begin();
  blynk_first_connect = true;
}
/*
void up() {
  String server_path = main_sever + "batch/update?token=" + BLYNK_AUTH_TOKEN
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
*/

BLYNK_WRITE(V0) {  //String
  String dataS = param.asStr();
  if (dataS == "active") {  //auto
    terminal.clear();
    key_set = true;
    Blynk.virtualWrite(V0, "Đã kích hoạt!");
  } else if (dataS == "deactive") {  //man
    terminal.clear();
    key_set = false;
    Blynk.virtualWrite(V0, "Hủy kích hoạt!");
  }
}
BLYNK_WRITE(V1) {  //Khu vực
  BlynkParamAllocated menu(128);
  switch (param.asInt()) {
    case 0:
      {  // Phuong 1
        khu_vuc = 1;
        menu.add("1");
        menu.add("2");
        menu.add("3");
        Blynk.setProperty(V2, "labels", menu);
        break;
      }
    case 1:
      {  // Phuong 2
        khu_vuc = 2;
        menu.add("Cầu cửa đông");
        menu.add("Ao lục bình");
        menu.add("3");
        Blynk.setProperty(V2, "labels", menu);
        break;
      }
    case 2:
      {  // Phuong 3
        khu_vuc = 3;
        menu.add("1");
        menu.add("2");
        menu.add("3");
        Blynk.setProperty(V2, "labels", menu);
        break;
      }
  }
}
BLYNK_WRITE(V2) {  //Địa điểm
  BlynkParamAllocated menu(128);
  switch (param.asInt()) {
    case 0:
      {
        dia_diem = 1;
        if (khu_vuc == 1) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  // Cầu cửa đông
          menu.add("Van 1");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 3) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        }
        break;
      }
    case 1:
      {
        dia_diem = 2;
        if (khu_vuc == 1) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  //Ao lục bình
          menu.add("Van 1");
          menu.add("Van 2");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 3) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        }
        break;
      }
  }
}
BLYNK_WRITE(V3) {  //Chọn van
  switch (param.asInt()) {
    case 0:
      {
        van = 1;
        break;
      }
    case 1:
      {
        van = 2;
        break;
      }
    case 2:
      {
        van = 3;
        break;
      }
    case 3:
      {
        van = 4;
        break;
      }
    case 4:
      {
        van = 5;
        break;
      }
  }
}
BLYNK_WRITE(V4) {  //Time input
  if (key_set) {
    TimeInputParam t(param);
    if (t.hasStartTime()) {
      start_ = t.getStartHour() * 3600 + t.getStartMinute() * 60;
    }
    if (t.hasStopTime()) {
      stop_ = t.getStopHour() * 3600 + t.getStopMinute() * 60;
    }
  }
}
BLYNK_WRITE(V5) {  //Save time input
  if (param.asInt() == 1) {
    if (khu_vuc == 2) {
      if (dia_diem == 1) {
        if (van == 1) {
          String server_path = salve_sever + "external/api/update/?token=" + caucuadong_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz;
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        }
      }
    }
  }
}
//-------------------------
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
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  //-----------------------
  delay(10000);

  timer.setInterval(900005L, []() {
    connectionstatus();
  });
}
void loop() {
  Blynk.run();
  timer.run();
}
