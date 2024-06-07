/*
V0 - String
V1 - Khu vực
V2 - Địa điểm
V3 - Chọn van
V4 - Time input
V5 - Save time
/////////// Cầu cửa đông
V6 - DATAS ccd
v7 - Btn van 1
V8 - Mode
V9 - G
/////////// UBND P2
V10 - DATAS ubnd p2
V11 - G
V12 - mode
V13 - Btn van 1
/////////// Ao lục bìnhk
V14 - DATAS aolucbinh
V15 - G
V16 - mode
V17 - Btn van 1
V18 - Btn van 2
/////////// Bờ kè 1
V19 - DATAS boke1
V20 - G
V21 - mode
V22 - Btn van 1
/////////// Nguyễn Thái Bình
V23 - DATAS ntbinh
V24 - G
V25 - mode
V26 - Btn van 1
/////////// Đường Hùng Vương
V27 - DATAS dhvuong
V28 - G
V29 - mode
V30 - Btn van 1
/////////// Trường THPT 1
V31 - DATAS thpt1
V32 - G
V33 - mode
V34 - Btn van 1
/////////// Trường THPT 2
V35 - DATAS thpt2
V36 - G
V37 - mode
V38 - Btn van 1

*/

#define BLYNK_TEMPLATE_ID "TMPL67uSt4c-z"
#define BLYNK_TEMPLATE_NAME "ĐÔ THỊ"
#define BLYNK_AUTH_TOKEN "w3ZZc7F4pvOIwqozyrzYcBFVUE3XxSiW"
#define BLYNK_FIRMWARE_VERSION "280508"

#define ccd_TOKEN "jaQFoaOgdcZcKbyI_ME_oi6tThEf4FR5"
#define ubndp2_TOKEN "gvfnRXv14oMohtqMWTPQXbduFKww1zfu"
#define alb_TOKEN "1v4Fr0n4m4-GaYP26MMZ3bHbTi5k68nP"
#define ntbinh_TOKEN "5xw1AG7yoX1e7sphhfL2jgc-dPnW9_l2"
#define boke1_TOKEN "Ip_LElKywC_tQiM7uChCt_5QKd_RfjWG"
#define boke2_TOKEN "I"
#define boke3_TOKEN "I"
#define dhvuong_TOKEN "eBeqi9ZJhRK3r66cUzgdD1gp2xGxG7kS"
#define thpt1_TOKEN "H3VsCxfjXq67ALREdZcKOANCQ_kdFqfg"
#define thpt2_TOKEN "H04PY3egc4KE3YnBQkOFEMNAGohM_oGo"

#define BLYNK_PRINT Serial
#define APP_DEBUG
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
const char* ssid = "Wifi_Modem";
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
//------------------- Cầu Cửa Đông
String pin_mode_ccd = "V8";
String pin_sta_ccd = "V6";
String pin_v1_ccd = "V7";
String pin_G_ccd = "V9";
bool sta_v1_ccd, mode_ccd;
int timer_sta_ccd;
byte hidden_key_ccd = 3;
//------------------- UBND P2
bool sta_v1_ubndp2, mode_ubndp2;
int timer_sta_ubndp2;
byte hidden_key_ubndp2 = 3;
//------------------- Ao lục bình
bool sta_v1_alb, sta_v2_alb;
bool mode_alb;
int timer_sta_alb;
byte hidden_key_alb = 3;
//------------------- Bờ kè 1
bool sta_v1_boke1;
bool mode_boke1;
int timer_sta_boke1;
byte hidden_key_boke1 = 3;
//------------------- Bờ kè 2
bool sta_v1_boke2;
bool mode_boke2;
int timer_sta_boke2;
byte hidden_key_boke2 = 3;
//------------------- Bờ kè 3
bool sta_v1_boke3;
bool mode_boke3;
int timer_sta_boke3;
byte hidden_key_boke3 = 3;
//------------------- N.T.Bình
bool sta_v1_ntbinh;
bool mode_ntbinh;
int timer_sta_ntbinh;
byte hidden_key_ntbinh = 3;
//------------------- D.H.Vuong
bool sta_v1_dhvuong;
bool mode_dhvuong;
int timer_sta_dhvuong;
byte hidden_key_dhvuong = 3;
//------------------- THPT 1
bool sta_v1_thpt1;
bool mode_thpt1;
int timer_sta_thpt1;
byte hidden_key_thpt1 = 3;
//------------------- THPT 2
bool sta_v1_thpt2;
bool mode_thpt2;
int timer_sta_thpt2;
byte hidden_key_thpt2 = 3;
//-------------------
WidgetRTC rtc_widget;
WidgetTerminal terminal(V0);
WidgetTerminal terminal_ccd(V6);
WidgetTerminal terminal_ubndp2(V10);
WidgetTerminal terminal_alb(V14);
WidgetTerminal terminal_boke(V19);
WidgetTerminal terminal_ntbinh(V23);
WidgetTerminal terminal_dhvuong(V27);
WidgetTerminal terminal_thpt1(V31);
WidgetTerminal terminal_thpt2(V35);
BlynkTimer timer;
BLYNK_CONNECTED() {
  rtc_widget.begin();
  blynk_first_connect = true;
}
//-------------------------------------------------------------------
void check_status() {
  String payload;
  String server_path;
  //-------- Cầu cửa đông
  {
    server_path = main_sever + "isHardwareConnected?token=" + ccd_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_ccd();
    } else {
      hidden_ccd();
    }
  }
  //-------- UBND P2
  {
    server_path = main_sever + "isHardwareConnected?token=" + ubndp2_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_ubndp2();
    } else {
      hidden_ubndp2();
    }
  }
  //-------- Ao lục bình
  {
    server_path = main_sever + "isHardwareConnected?token=" + alb_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_alb();
    } else {
      hidden_alb();
    }
  }
  //-------- N.T.Bình
  {
    server_path = main_sever + "isHardwareConnected?token=" + ntbinh_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_ntbinh();
    } else {
      hidden_ntbinh();
    }
  }
  //-------- Bờ kè 1
  {
    server_path = main_sever + "isHardwareConnected?token=" + boke1_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_boke1();
    } else {
      hidden_boke1();
    }
  }
  /*
  //-------- Bờ kè 2
  {
    server_path = main_sever + "isHardwareConnected?token=" + boke2_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_boke2();
    } else {
      hidden_boke2();}
  }
  //-------- Bờ kè 3
  {
    server_path = main_sever + "isHardwareConnected?token=" + boke3_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_boke3();
    } else {
      hidden_boke3();}
  }
  */
  //-------- Đường Hùng Vương
  {
    server_path = main_sever + "isHardwareConnected?token=" + dhvuong_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_dhvuong();
    } else {
      hidden_dhvuong();
    }
  }
  //-------- THPT 1
  {
    server_path = main_sever + "isHardwareConnected?token=" + thpt1_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_thpt1();
    } else {
      hidden_thpt1();
    }
  }
  //-------- THPT 2
  {
    server_path = main_sever + "isHardwareConnected?token=" + thpt2_TOKEN;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      payload = http.getString();
    }
    http.end();
    if (payload == "true") {
      visible_thpt2();
    } else {
      hidden_thpt2();
    }
  }
}
BLYNK_WRITE(V100) {
  String dataS = param.asStr();
  Blynk.logEvent("error", String(dataS));
}
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
  } else if (dataS == "caidat") {  //auto
    terminal.clear();
    key_set = true;
    timer.setTimeout(301000, []() {
      key_set = false;
      terminal.clear();
    });
    Blynk.virtualWrite(V0, "Đã kích hoạt Cài Đặt!");
  } else if (dataS == "luu") {  //auto
    terminal.clear();
    key_set = false;
    key = false;
    Blynk.virtualWrite(V0, "Đã lưu!");
  } else if (dataS == "update") {  //Update main
    terminal.clear();
    Blynk.virtualWrite(V0, "MAIN UPDATE...");
    update_fw();
  } else if (dataS == "update_p2_ccd") {  //update Cầu cửa đông
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "CCĐ UPDATE...");
  } else if (dataS == "rst_p2_ccd") {  //RST Cầu cửa đông
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "CCĐ RESTART...");
  } else if (dataS == "update_p2_alb") {  //update Ao lục bình
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "ALB UPDATE...");
  } else if (dataS == "rst_p2_alb") {  //RST Ao lục bình
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "ALB RESTART...");
  } else if (dataS == "update_p2_ubndp2") {  //update UBNDP2
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "UBNDP2 UPDATE...");
  } else if (dataS == "rst_p2_ubndp2") {  //RST UBNDP2
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "ubndp2 RESTART...");
  } else if (dataS == "update_p2_boke1") {  //update Bờ kè 1
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + boke1_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "Bờ kè 1 UPDATE...");
  } else if (dataS == "rst_p2_boke1") {  //RST Bờ kè 1
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + boke1_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "boke1 RESTART...");
  } else if (dataS == "update_p3_ntbinh") {  //update ntbinh
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + ntbinh_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "ntbinh UPDATE...");
  } else if (dataS == "rst_p3_ntbinh") {  //RST ntbinh
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + ntbinh_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "ntbinh RESTART...");
  } else if (dataS == "update_p3_dhvuong") {  //update dhvuong
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + dhvuong_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "dhvuong UPDATE...");
  } else if (dataS == "rst_p3_dhvuong") {  //RST dhvuong
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + dhvuong_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "dhvuong RESTART...");
  } else if (dataS == "update_p1_thpt1") {  //update thpt1
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + thpt1_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "thpt1 UPDATE...");
  } else if (dataS == "rst_p1_thpt1") {  //RST thpt1
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + thpt1_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "thpt1 RESTART...");
  } else if (dataS == "update_p1_thpt2") {  //update thpt2
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + thpt2_TOKEN
                         + "&V0=" + "update";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "thpt2 UPDATE...");
  } else if (dataS == "rst_p1_thpt2") {  //RST thpt2
    terminal.clear();
    String server_path = main_sever + "batch/update?token=" + thpt2_TOKEN
                         + "&V0=" + "rst";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
    Blynk.virtualWrite(V0, "thpt2 RESTART...");
  }
}
BLYNK_WRITE(V1) {  //Khu vực
  BlynkParamAllocated menu(128);
  switch (param.asInt()) {
    case 0:
      {  // Phuong 1
        khu_vuc = 1;
        menu.add("Trường THPT 1");
        menu.add("Trường THPT 2");
        Blynk.setProperty(V2, "labels", menu);
        break;
      }
    case 1:
      {  // Phuong 2
        khu_vuc = 2;
        menu.add("Cầu cửa đông");
        menu.add("Ao lục bình");
        menu.add("UBND P2");
        menu.add("Bờ kè 1");
        menu.add("Bờ kè 2");
        menu.add("Bờ kè 3");
        Blynk.setProperty(V2, "labels", menu);
        break;
      }
    case 2:
      {  // Phuong 3
        khu_vuc = 3;
        menu.add("N.T.Bình");
        menu.add("Đ.H.Vương");
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
        if (khu_vuc == 1) {  // Trường THPT 1
          menu.add("Van 1");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  // Cầu cửa đông
          menu.add("Van 1");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 3) {  // Nguyễn Thái Bình
          menu.add("Van 1");
          Blynk.setProperty(V3, "labels", menu);
        }
        break;
      }
    case 1:
      {
        dia_diem = 2;
        if (khu_vuc == 1) {  // Trường THPT 2
          menu.add("Van 1");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  //Ao lục bình
          menu.add("Van 1");
          menu.add("Van 2");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 3) {  //D.H.Vuong
          menu.add("Van 1");
          Blynk.setProperty(V3, "labels", menu);
        }
        break;
      }
    case 2:
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
    case 3:
      {
        dia_diem = 4;
        if (khu_vuc == 1) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  //Bờ kè 1
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
    case 4:
      {
        dia_diem = 5;
        if (khu_vuc == 1) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  //Bờ kè 2
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
    case 5:
      {
        dia_diem = 6;
        if (khu_vuc == 1) {  // ...
          menu.add("Van 1");
          menu.add("Van 2");
          menu.add("Van 3");
          Blynk.setProperty(V3, "labels", menu);
        } else if (khu_vuc == 2) {  //Bờ kè 3
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
  Blynk.virtualWrite(V3, 100);
}
BLYNK_WRITE(V3) {  //Chọn van
  switch (param.asInt()) {
    case 0:
      {
        van = 1;
        if (khu_vuc == 1) {
          if (dia_diem == 1) {
            String server_path = main_sever + "batch/update?token=" + thpt1_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 2) {
            String server_path = main_sever + "batch/update?token=" + thpt2_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          }
        } else if (khu_vuc == 2) {
          if (dia_diem == 1) {
            String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
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
          } else if (dia_diem == 4) {
            String server_path = main_sever + "batch/update?token=" + boke1_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 5) {
            String server_path = main_sever + "batch/update?token=" + boke2_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 6) {
            String server_path = main_sever + "batch/update?token=" + boke3_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          }
        } else if (khu_vuc == 3) {
          if (dia_diem == 1) {
            String server_path = main_sever + "batch/update?token=" + ntbinh_TOKEN
                                 + "&V0=" + "van1";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 2) {
            String server_path = main_sever + "batch/update?token=" + dhvuong_TOKEN
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
        if (khu_vuc == 2) {
          if (dia_diem == 1) {
            String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
                                 + "&V0=" + "van2";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 2) {
            String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                                 + "&V0=" + "van2";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          } else if (dia_diem == 3) {
            String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                                 + "&V0=" + "van2";
            http.begin(client, server_path.c_str());
            int httpResponseCode = http.GET();
            http.end();
          }
        }
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
  if (key_set) {
    if (param.asInt() == 1) {
      if (khu_vuc == 1) {
        if (dia_diem == 1) {  //THPT 1
          String server_path = main_sever + "batch/update?token=" + thpt1_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
          terminal.clear();
        } else if (dia_diem == 2) {  //THPT 2
          String server_path = main_sever + "batch/update?token=" + thpt2_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
          terminal.clear();
        }
      } else if (khu_vuc == 2) {
        if (dia_diem == 1) {  //Cầu cửa đông
          String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
          terminal.clear();
        } else if (dia_diem == 2) {  //Ao lục bình
          String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        } else if (dia_diem == 3) {  //UBND P2
          String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        } else if (dia_diem == 4) {  //Bờ kè 1
          String server_path = main_sever + "batch/update?token=" + boke1_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        } else if (dia_diem == 5) {  //Bờ kè 2
          String server_path = main_sever + "batch/update?token=" + boke2_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        } else if (dia_diem == 6) {  //Bờ kè 3
          String server_path = main_sever + "batch/update?token=" + boke3_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        }
      } else if (khu_vuc == 3) {
        if (dia_diem == 1) {  //N.T.Bình
          String server_path = main_sever + "batch/update?token=" + ntbinh_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        } else if (dia_diem == 2) {  //D.H.Vương
          String server_path = main_sever + "batch/update?token=" + dhvuong_TOKEN
                               + "&V1=" + start_
                               + "&V1=" + stop_
                               + "&V1=" + tz
                               + "&V1=" + String(A);
          http.begin(client, server_path.c_str());
          int httpResponseCode = http.GET();
          http.end();
        }
      }
    }
  } else {
    terminal.clear();
    Blynk.virtualWrite(V0, "Hãy nhập mật mã trước khi cài đặt!");
  }
}
//------------------- Cầu Cửa Đông
void hidden_ccd() {
  if (hidden_key_ccd != true) {
    Blynk.setProperty(V6, V8, V7, "isDisabled", "true");
    hidden_key_ccd = true;
  }
}
void visible_ccd() {
  if (hidden_key_ccd != false) {
    Blynk.setProperty(V6, V8, V7, "isDisabled", "false");
    hidden_key_ccd = false;
  }
}
BLYNK_WRITE(V6) {
  String dataS = param.asStr();
  if ((dataS == "dothi") || (dataS == "dothi ") || (dataS == " dothi ") || (dataS == " dothi")) {  //man
    terminal_ccd.clear();
    key = true;
    Blynk.virtualWrite(V6, "OK!\nKích hoạt trong 10s");
    timer.setTimeout(10000, []() {
      key = false;
      terminal_ccd.clear();
    });
  } else if ((dataS == "1") || (dataS == " 1") || (dataS == "1 ") || (dataS == " 1 ")) {
    terminal_ccd.clear();
    String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
                         + "&V0=" + "info";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  }
}
BLYNK_WRITE(V7) {  //Btn Van 1
  if ((key) && (mode_ccd == 0)) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V7, sta_v1_ccd);
}
BLYNK_WRITE(V8) {  //mode
  String dataX;
  if (key) {
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
    String server_path = main_sever + "batch/update?token=" + ccd_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V8, mode_ccd);
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
void hidden_ubndp2() {
  if (hidden_key_ubndp2 != true) {
    Blynk.setProperty(V10, V12, V13, "isDisabled", "true");
    hidden_key_ubndp2 = true;
  }
}
void visible_ubndp2() {
  if (hidden_key_ubndp2 != false) {
    Blynk.setProperty(V10, V12, V13, "isDisabled", "false");
    hidden_key_ubndp2 = false;
  }
}
BLYNK_WRITE(V10) {
  String dataS = param.asStr();
  if ((dataS == "dothi") || (dataS == "dothi ") || (dataS == " dothi ") || (dataS == " dothi")) {  //man
    terminal_ubndp2.clear();
    key = true;
    Blynk.virtualWrite(V10, "OK!\nKích hoạt trong 10s");
    timer.setTimeout(10000, []() {
      key = false;
      terminal_ubndp2.clear();
    });
  } else if ((dataS == "1") || (dataS == " 1") || (dataS == "1 ") || (dataS == " 1 ")) {
    terminal_ubndp2.clear();
    String server_path = main_sever + "batch/update?token=" + ubndp2_TOKEN
                         + "&V0=" + "info";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  }
}
BLYNK_WRITE(V13) {  //Btn Van 1
  if ((key) && (mode_ubndp2 == 0)) {
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
  if (key) {
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
  } else Blynk.virtualWrite(V12, mode_ubndp2);
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
void hidden_alb() {
  if (hidden_key_alb != true) {
    Blynk.setProperty(V14, V16, V17, V18, "isDisabled", "true");
    hidden_key_alb = true;
  }
}
void visible_alb() {
  if (hidden_key_alb != false) {
    Blynk.setProperty(V14, V16, V17, V18, "isDisabled", "false");
    hidden_key_alb = false;
  }
}
BLYNK_WRITE(V14) {
  String dataS = param.asStr();
  if ((dataS == "dothi") || (dataS == "dothi ") || (dataS == " dothi ") || (dataS == " dothi")) {  //man
    terminal_alb.clear();
    key = true;
    Blynk.virtualWrite(V14, "OK!\nKích hoạt trong 10s");
    timer.setTimeout(10000, []() {
      key = false;
      terminal_alb.clear();
    });
  } else if ((dataS == "1") || (dataS == " 1") || (dataS == "1 ") || (dataS == " 1 ")) {
    terminal_alb.clear();
    String server_path = main_sever + "batch/update?token=" + alb_TOKEN
                         + "&V0=" + "info";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
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
  if (key) {
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
  } else Blynk.virtualWrite(V16, mode_alb);
}
BLYNK_WRITE(V17) {  //Btn Van 1
  if ((key) && (mode_alb == 0)) {
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
  if ((key) && (mode_alb == 0)) {
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
//------------------- Bờ kè 1
void hidden_boke1() {
  if (hidden_key_boke1 != true) {
    Blynk.setProperty(V21, V22, "isDisabled", "true");
    hidden_key_boke1 = true;
  }
}
void visible_boke1() {
  if (hidden_key_boke1 != false) {
    Blynk.setProperty(V21, V22, "isDisabled", "false");
    hidden_key_boke1 = false;
  }
}
BLYNK_WRITE(V19) {  //
}
BLYNK_WRITE(V20) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 2; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_boke1 != bit) {
          mode_boke1 = bit;
          Blynk.virtualWrite(V21, mode_boke1);
        }
        break;
      case 1:
        if (sta_v1_boke1 != bit) {
          sta_v1_boke1 = bit;
          Blynk.virtualWrite(V22, sta_v1_boke1);
        }
        break;
    }
  }
}
BLYNK_WRITE(V21) {  //mode
  String dataX;
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_boke1 = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_boke1 = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + boke1_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else {
    dataX = "mode";
    String server_path = main_sever + "batch/update?token=" + boke1_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  }
}
BLYNK_WRITE(V22) {  //Btn Van 1
  if (key) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + boke1_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V22, sta_v1_boke1);
}
//------------------- Nguyễn Thái Bình
void hidden_ntbinh() {
  if (hidden_key_ntbinh != true) {
    Blynk.setProperty(V23, V25, V26, "isDisabled", "true");
    hidden_key_ntbinh = true;
  }
}
void visible_ntbinh() {
  if (hidden_key_ntbinh != false) {
    Blynk.setProperty(V23, V25, V26, "isDisabled", "false");
    hidden_key_ntbinh = false;
  }
}
BLYNK_WRITE(V23) {
  String dataS = param.asStr();
  if ((dataS == "dothi") || (dataS == "dothi ") || (dataS == " dothi ") || (dataS == " dothi")) {  //man
    terminal_ntbinh.clear();
    key = true;
    Blynk.virtualWrite(V23, "OK!\nKích hoạt trong 10s");
    timer.setTimeout(10000, []() {
      key = false;
      terminal_ntbinh.clear();
    });
  } else if ((dataS == "1") || (dataS == " 1") || (dataS == "1 ") || (dataS == " 1 ")) {
    terminal_ntbinh.clear();
    String server_path = main_sever + "batch/update?token=" + ntbinh_TOKEN
                         + "&V0=" + "info";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  }
}
BLYNK_WRITE(V24) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 2; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_ntbinh != bit) {
          mode_ntbinh = bit;
          Blynk.virtualWrite(V25, mode_ntbinh);
        }
        break;
      case 1:
        if (sta_v1_ntbinh != bit) {
          sta_v1_ntbinh = bit;
          Blynk.virtualWrite(V26, sta_v1_ntbinh);
        }
        break;
    }
  }
}
BLYNK_WRITE(V25) {  //mode
  String dataX;
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_ntbinh = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_ntbinh = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + ntbinh_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V25, mode_ntbinh);
}
BLYNK_WRITE(V26) {  //Btn Van 1
  if ((key) && (mode_ntbinh == 0)) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + ntbinh_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V26, sta_v1_ntbinh);
}
//------------------- Đường Hùng Vương
void hidden_dhvuong() {
  if (hidden_key_dhvuong != true) {
    Blynk.setProperty(V27, V29, V30, "isDisabled", "true");
    hidden_key_dhvuong = true;
  }
}
void visible_dhvuong() {
  if (hidden_key_dhvuong != false) {
    Blynk.setProperty(V27, V29, V30, "isDisabled", "false");
    hidden_key_dhvuong = false;
  }
}
BLYNK_WRITE(V27) {
  String dataS = param.asStr();
  if ((dataS == "dothi") || (dataS == "dothi ") || (dataS == " dothi ") || (dataS == " dothi")) {  //man
    terminal_dhvuong.clear();
    key = true;
    Blynk.virtualWrite(V27, "OK!\nKích hoạt trong 10s");
    timer.setTimeout(10000, []() {
      key = false;
      terminal_dhvuong.clear();
    });
  } else if ((dataS == "1") || (dataS == " 1") || (dataS == "1 ") || (dataS == " 1 ")) {
    terminal_dhvuong.clear();
    String server_path = main_sever + "batch/update?token=" + dhvuong_TOKEN
                         + "&V0=" + "info";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  }
}
BLYNK_WRITE(V28) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 2; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_dhvuong != bit) {
          mode_dhvuong = bit;
          Blynk.virtualWrite(V29, mode_dhvuong);
        }
        break;
      case 1:
        if (sta_v1_dhvuong != bit) {
          sta_v1_dhvuong = bit;
          Blynk.virtualWrite(V30, sta_v1_dhvuong);
        }
        break;
    }
  }
}
BLYNK_WRITE(V29) {  //mode
  String dataX;
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_dhvuong = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_dhvuong = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + dhvuong_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V29, mode_dhvuong);
}
BLYNK_WRITE(V30) {  //Btn Van 1
  if ((key) && (mode_dhvuong == 0)) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + dhvuong_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V30, sta_v1_dhvuong);
}
//------------------- THPT 1 (Bên Trái)
void hidden_thpt1() {
  if (hidden_key_thpt1 != true) {
    Blynk.setProperty(V31, V33, V34, "isDisabled", "true");
    hidden_key_thpt1 = true;
  }
}
void visible_thpt1() {
  if (hidden_key_thpt1 != false) {
    Blynk.setProperty(V31, V33, V34, "isDisabled", "false");
    hidden_key_thpt1 = false;
  }
}
BLYNK_WRITE(V31) {
  String dataS = param.asStr();
  if ((dataS == "dothi") || (dataS == "dothi ") || (dataS == " dothi ") || (dataS == " dothi")) {  //man
    terminal_thpt1.clear();
    key = true;
    Blynk.virtualWrite(V31, "OK!\nKích hoạt trong 10s");
    timer.setTimeout(10000, []() {
      key = false;
      terminal_thpt1.clear();
    });
  } else if ((dataS == "1") || (dataS == " 1") || (dataS == "1 ") || (dataS == " 1 ")) {
    terminal_thpt1.clear();
    String server_path = main_sever + "batch/update?token=" + thpt1_TOKEN
                         + "&V0=" + "info";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  }
}
BLYNK_WRITE(V32) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 2; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_thpt1 != bit) {
          mode_thpt1 = bit;
          Blynk.virtualWrite(V33, mode_thpt1);
        }
        break;
      case 1:
        if (sta_v1_thpt1 != bit) {
          sta_v1_thpt1 = bit;
          Blynk.virtualWrite(V34, sta_v1_thpt1);
        }
        break;
    }
  }
}
BLYNK_WRITE(V33) {  //mode
  String dataX;
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_thpt1 = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_thpt1 = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + thpt1_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V33, mode_thpt1);
}
BLYNK_WRITE(V34) {  //Btn Van 1
  if ((key) && (mode_thpt1 == 0)) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + thpt1_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V34, sta_v1_thpt1);
}
//------------------- THPT 2 (Bên Phải)
void hidden_thpt2() {
  if (hidden_key_thpt2 != true) {
    Blynk.setProperty(V35, V37, V38, "isDisabled", "true");
    hidden_key_thpt2 = true;
  }
}
void visible_thpt2() {
  if (hidden_key_thpt2 != false) {
    Blynk.setProperty(V35, V37, V38, "isDisabled", "false");
    hidden_key_thpt2 = false;
  }
}
BLYNK_WRITE(V35) {
  String dataS = param.asStr();
  if ((dataS == "dothi") || (dataS == "dothi ") || (dataS == " dothi ") || (dataS == " dothi")) {  //man
    terminal_thpt2.clear();
    key = true;
    Blynk.virtualWrite(V35, "OK!\nKích hoạt trong 10s");
    timer.setTimeout(10000, []() {
      key = false;
      terminal_thpt2.clear();
    });
  } else if ((dataS == "1") || (dataS == " 1") || (dataS == "1 ") || (dataS == " 1 ")) {
    terminal_thpt2.clear();
    String server_path = main_sever + "batch/update?token=" + thpt2_TOKEN
                         + "&V0=" + "info";
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  }
}
BLYNK_WRITE(V36) {  //data
  byte G = param.asInt();
  for (byte i = 0; i < 2; i++) {
    byte bit = G % 2;
    G /= 2;
    switch (i) {
      case 0:
        if (mode_thpt2 != bit) {
          mode_thpt2 = bit;
          Blynk.virtualWrite(V37, mode_thpt2);
        }
        break;
      case 1:
        if (sta_v1_thpt2 != bit) {
          sta_v1_thpt2 = bit;
          Blynk.virtualWrite(V38, sta_v1_thpt2);
        }
        break;
    }
  }
}
BLYNK_WRITE(V37) {  //mode
  String dataX;
  if (key) {
    switch (param.asInt()) {
      case 0:
        {  // Man
          dataX = "m";
          mode_thpt2 = 0;
          break;
        }
      case 1:
        {  // Auto
          dataX = "a";
          mode_thpt2 = 1;
          break;
        }
    }
    String server_path = main_sever + "batch/update?token=" + thpt2_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V37, mode_thpt2);
}
BLYNK_WRITE(V38) {  //Btn Van 1
  if ((key) && (mode_thpt2 == 0)) {
    String dataX;
    if (param.asInt() == HIGH) {
      dataX = "van1_on";
    } else dataX = "van1_off";
    String server_path = main_sever + "batch/update?token=" + thpt2_TOKEN
                         + "&V0=" + dataX;
    http.begin(client, server_path.c_str());
    int httpResponseCode = http.GET();
    http.end();
  } else Blynk.virtualWrite(V38, sta_v1_thpt2);
}
//-------------------------------------------------------------------
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
  timer.setInterval(5000, check_status);
}
void loop() {
  Blynk.run();
  timer.run();
}