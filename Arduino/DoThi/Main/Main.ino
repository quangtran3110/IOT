/*
V0 - String
V1 - Khu vực
V2 - Địa điểm
V3 - Chọn van
V4 - Time input
V5 - Save time
/////////// Cầu cửa đông
V6 - Sta caucuadong
v7 - Btn van 1
V8 - Mode
V9 - G
/////////// UBND P2
V10 - Sta ubnd p2
V11 - G
V12 - mode
V13 - Btn van 1
/////////// Ao lục bình
V14 - Sta aolucbinh
V15 - G
V16 - mode
V17 - Btn van 1
V18 - Btn van 2

*/

#define BLYNK_TEMPLATE_ID "TMPL6LdmF6nY7"
#define BLYNK_TEMPLATE_NAME "Đô Thị"
#define BLYNK_AUTH_TOKEN "Oyy7F8HDxVurrNg0QOSS6gjsCSQTsDqZ"
#define BLYNK_FIRMWARE_VERSION "240116"

#define caucuadong_TOKEN "jaQFoaOgdcZcKbyI_ME_oi6tThEf4FR5"
#define ubndp2_TOKEN "gvfnRXv14oMohtqMWTPQXbduFKww1zfu"
#define alb_TOKEN "1v4Fr0n4m4-GaYP26MMZ3bHbTi5k68nP"  //Ao lục bình


#define BLYNK_PRINT Serial
#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
const char* ssid = "Wifi";
const char* password = "Password";
//const char* ssid = "tram bom so 4";
//const char* password = "0943950555";

#include <WidgetRTC.h>
char daysOfTheWeek[7][12] = { "CN", "T2", "T3", "T4", "T5", "T6", "T7" };
char tz[] = "Asia/Ho_Chi_Minh";

#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
#define URL_fw_Bin "https://raw.githubusercontent.com/quangtran3110/IOT/main/Arduino/DoThi/Main/build/esp8266.esp8266.nodemcuv2/Main.ino.bin"
String main_sever = "http://sgp1.blynk.cloud/external/api/";

int khu_vuc = 0, dia_diem = 0, van = 0;
uint32_t start_, stop_;
byte t2, t3, t4, t5, t6, t7, cn;
char A[50] = "";
byte reboot_num;
bool blynk_first_connect = false, key_set = false, key = false;




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
    key = true;
    Blynk.virtualWrite(V0, "Đã kích hoạt!");
  } else if (dataS == "deactive") {  //man
    terminal.clear();
    key_set = false;
    key = false;
    Blynk.virtualWrite(V0, "Hủy kích hoạt!");
  } else if (dataS == "dothi") {  //man
    terminal.clear();
    key = true;
    Blynk.virtualWrite(V0, "OK!\nKích hoạt trong 15s");
    timer.setTimeout(15000, []() {
      key = false;
      terminal.clear();
    });
  } else if (dataS == "update") {  //Update main
    terminal.clear();
    update_fw();
  } else if (dataS == "update_p2_ccd") {  //update Cầu cửa đông
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + caucuadong_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else if (dataS == "update_p2_alb") {  //update Cầu cửa đông
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
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
        menu.add("UBND P2");
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
    case 3:
      {
        dia_diem = 3;
        if (khu_vuc == 1) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  //UBND P2
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
  }
}
BLYNK_WRITE(V3) {  //Chọn van
  switch (param.asInt()) {
    case 0:
      {
        van = 1;
        if (khu_vuc == 2) {
          if (dia_diem == 1) {
            String server_path = main_sever + "batch/update?token=" + caucuadong_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 2) {
            String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 3) {
            String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          }
        }
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
    memset(A, '\0', sizeof(A));
    for (int i = 1; i <= 7; i++) {
      // Nếu ngày i được chọn
      if (t.isWeekdaySelected(i) == 1) {
        // Thêm giá trị i vào mảng A
        strcat(A, String(i).c_str());
        strcat(A, ",");
      }
    }
    // Xóa ký tự cuối cùng là dấu phẩy
    A[strlen(A) - 1] = '\0';
  }
}
BLYNK_WRITE(V5) {  //Save time input
  if (param.asInt() == 1) {
    if (khu_vuc == 2) {
      if (dia_diem == 1) {  //Cầu cửa đông
        if (van == 1) {
          String server_path = main_sever + "batch/update?token=" + caucuadong_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        }
      } else if (dia_diem == 2) {  //Ao lục bình
        if (van == 1) {
          String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz;
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        }
      } else if (dia_diem == 3) {  //UBND P2
        if (van == 1) {
          String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
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
//------------------- Cầu Cửa Đông
bool sta_v1_ccd, mode_ccd;
int timer_sta_ccd;
byte sta_ccd;
bool hidden_key_ccd = false;
void hidden_ccd() {
  if (hidden_key_ccd == false) {
    Blynk.setProperty(V8, V7, "isDisabled", "true");
    hidden_key_ccd = true;
  }
}
void visible_ccd() {
  if (hidden_key_ccd == true) {
    Blynk.setProperty(V8, V7, "isDisabled", "false");
    hidden_key_ccd = false;
  }
}
BLYNK_WRITE(V6) {  //Status Cầu cửa đông
  sta_ccd = param.asInt();
  if (sta_ccd == 1) {
    visible_ccd();
    timer.restartTimer(timer_sta_ccd);
  }
}
BLYNK_WRITE(V7) {  //Btn Van 1
  if (key) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + caucuadong_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V7, sta_v1_ccd);
}
BLYNK_WRITE(V8) {  //mode
  String dataX;
  if ((key) && (sta_ccd == 1)) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_ccd = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_ccd = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + caucuadong_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else {
    if (sta_ccd == 1) {
      dataX = "mode";
      String server_path = main_sever + "batch/update?token=" + caucuadong_TOKEN
                           + "&V0=" + dataX;
      http.begin(client, server_path.c_str());
      int httpResponseCode = http.GET();
      http.end();
    }
  }
}
BLYNK_WRITE(V9) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 2; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_ccd != bit) {
          mode_ccd = bit;
          Blynk.virtualWrite(V8, mode_ccd);
        }
        break;
      case 1:
        if (sta_v1_ccd != bit) {
          sta_v1_ccd = bit;
          Blynk.virtualWrite(V7, sta_v1_ccd);
        }
        break;
    }
  }
}
//------------------- UBND P2
bool sta_v1_ubndp2, mode_ubndp2;
int timer_sta_ubndp2;
byte sta_ubndp2;
bool hidden_key_ubndp2 = false;
void hidden_ubndp2() {
  if (hidden_key_ubndp2 == false) {
    Blynk.setProperty(V12, V13, "isDisabled", "true");
    hidden_key_ubndp2 = true;
  }
}
void visible_ubndp2() {
  if (hidden_key_ubndp2 == true) {
    Blynk.setProperty(V12, V13, "isDisabled", "false");
    hidden_key_ubndp2 = false;
  }
}
BLYNK_WRITE(V10) {  //Status Cầu cửa đông
  sta_ubndp2 = param.asInt();
  if (sta_ubndp2 == 1) {
    visible_ubndp2();
    timer.restartTimer(timer_sta_ubndp2);
  }
}
BLYNK_WRITE(V13) {  //Btn Van 1
  if (key) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V13, sta_v1_ubndp2);
}
BLYNK_WRITE(V12) {  //mode
  String dataX;
  if ((key) && (sta_ubndp2 == 1)) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_ubndp2 = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_ubndp2 = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else {
    if (sta_ubndp2 == 1) {
      dataX = "mode";
      String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                           + "&V0=" + dataX;
      http.begin(client, server_path.c_str());
      int httpResponseCode = http.GET();
      http.end();
    }
  }
}
BLYNK_WRITE(V11) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 2; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_ubndp2 != bit) {
          mode_ubndp2 = bit;
          Blynk.virtualWrite(V12, mode_ubndp2);
        }
        break;
      case 1:
        if (sta_v1_ubndp2 != bit) {
          sta_v1_ubndp2 = bit;
          Blynk.virtualWrite(V13, sta_v1_ubndp2);
        }
        break;
    }
  }
}
//------------------- Ao lục bình
bool sta_v1_alb, sta_v2_alb;
bool mode_alb;
int timer_sta_alb;
byte sta_alb;
bool hidden_key_alb = false;
void hidden_alb() {
  if (hidden_key_alb == false) {
    Blynk.setProperty(V16, V17, V18, "isDisabled", "true");
    hidden_key_alb = true;
  }
}
void visible_alb() {
  if (hidden_key_alb == true) {
    Blynk.setProperty(V16, V17, V18, "isDisabled", "false");
    hidden_key_alb = false;
  }
}
BLYNK_WRITE(V14) {  //Status Ao lục bình
  sta_alb = param.asInt();
  if (sta_alb == 1) {
    visible_alb();
    timer.restartTimer(timer_sta_alb);
  }
}
BLYNK_WRITE(V15) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 3; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_alb != bit) {
          mode_alb = bit;
          Blynk.virtualWrite(V16, mode_alb);
        }
        break;
      case 1:
        if (sta_v1_alb != bit) {
          sta_v1_alb = bit;
          Blynk.virtualWrite(V17, sta_v1_alb);
        }
        break;
      case 2:
        if (sta_v2_alb != bit) {
          sta_v2_alb = bit;
          Blynk.virtualWrite(V18, sta_v2_alb);
        }
        break;
    }
  }
}
BLYNK_WRITE(V16) {  //mode
  String dataX;
  if ((key) && (sta_alb == 1)) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_alb = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_alb = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else {
    if (sta_alb == 1) {
      dataX = "mode";
      String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                           + "&V0=" + dataX;
      http.begin(client, server_path.c_str());
      int httpResponseCode = http.GET();
      http.end();
    }
  }
}
BLYNK_WRITE(V17) {  //Btn Van 1
  if (key) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V17, sta_v1_alb);
}
BLYNK_WRITE(V18) {  //Btn Van 2
  if (key) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van2_on";
    } else dataX = "van2_off";
    String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V18, sta_v2_alb);
}
//-------------------
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
  //-----------------------
  delay(10000);

  timer.setInterval(900005L, []() {
    connectionstatus();
  });
  timer_sta_ccd = timer.setInterval(30018, hidden_ccd);
  timer_sta_alb = timer.setInterval(30115, hidden_alb);
  timer_sta_ubndp2 = timer.setInterval(30045, hidden_ubndp2);
}
void loop() {
  Blynk.run();
  timer.run();
}