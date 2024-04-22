#define BLYNK_TEMPLATE_ID "TMPL6I6ISEvF5"
#define BLYNK_TEMPLATE_NAME "SUPPORT 1"
#define BLYNK_AUTH_TOKEN "mAEloc4FYavbw8Jh8KPbhJSjUGWyxKqn"
#define BLYNK_PRINT Serial
#define APP_DEBUG

const char* ssid = "Hiddennet";
const char* password = "Password";
//const char* ssid = "tram bom so 4";
//const char* password = "0943950555";

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
String server_name = "http://sgp1.blynk.cloud/external/api/batch/update?token=ESzia3fpA-29cs8gt85pGnrPq_rICcqf&";  // <<< SEE COMMENTS

bool statusRualoc1 = LOW, statusRualoc3 = LOW;
int rualoc1, rualoc3;

BlynkTimer timer;
BLYNK_CONNECTED() {
}

const int rl1 = D1;
const int rl2 = D2;
const int rl3 = D5;
const int rl4 = D6;

void status() {
  //String server_path = server_name + "V10=" + a + "&V2=" + b + "&V3=" + c;
  String server_path = server_name + "V39=" + 1;
  http.begin(client, server_path.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
  }
  http.end();
}

void loc1() {
  if (statusRualoc1 == HIGH) {
    digitalWrite(rl1, LOW);
    digitalWrite(rl2, LOW);
    timer.setTimeout((6 * 60 * 1000), []() {
      digitalWrite(rl1, HIGH);
      digitalWrite(rl2, HIGH);
    });
  }
}
void loc3() {
  if (statusRualoc3 == HIGH) {
    digitalWrite(rl3, LOW);
    timer.setTimeout((5 * 60 * 1000), []() {
      digitalWrite(rl3, HIGH);
    });
  }
}

BLYNK_WRITE(V0) {
  if (param.asInt() == 0) {
    statusRualoc1 = LOW;
    statusRualoc3 = LOW;

  } else if (param.asInt() == 1) {
    statusRualoc1 = HIGH;
    statusRualoc3 = LOW;
    //timer.restartTimer(rualoc1);
  } else if (param.asInt() == 2) {
    statusRualoc3 = HIGH;
    statusRualoc1 = LOW;
    //timer.restartTimer(rualoc3);
  } else if (param.asInt() == 3) {
    statusRualoc3 = HIGH;
    statusRualoc1 = HIGH;
  }
  Serial.println(statusRualoc1);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Blynk.config(BLYNK_AUTH_TOKEN);
  delay(5000);

  pinMode(rl1, OUTPUT);
  digitalWrite(rl1, HIGH);
  pinMode(rl2, OUTPUT);
  digitalWrite(rl2, HIGH);
  pinMode(rl3, OUTPUT);
  digitalWrite(rl3, HIGH);
  pinMode(rl4, OUTPUT);
  digitalWrite(rl4, HIGH);
  //timer.setInterval(1503L, updata);
  //timer.setInterval(200L, readPressure);
  timer.setInterval((10 * 60 * 1000), loc1);
  timer.setInterval((10 * 61 * 1000), loc3);
  timer.setInterval(5013, status);
}
void loop() {
  Blynk.run();
  timer.run();
}