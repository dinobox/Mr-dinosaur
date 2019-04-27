#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <string.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
// To run, set your ESP8266 build to 160MHz, update the SSID info, and upload.
// Enter your WiFi setup here:
const char *SSID = "hashfish";
const char *PASSWORD = "56cf12d8";
// Randomly picked URL
AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2SNoDAC *out;
ESP8266WebServer server(80);
String pm3file = "";
String token = "";
void gettoken() {
  WiFiClient client;
  HTTPClient http;
  String value = server.arg(0);
  if (http.begin(client, "http://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=18PUVnFTdVn49bdMuBNr0sZc&client_secret=jU04KneI35AFKFpgjvaPTYEZE15sQTUH")) {  // HTTP
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        payload = payload.substring(payload.indexOf("access_token") + 15);
        token = payload.substring(0, payload.indexOf("session_key") - 3);
        Serial.println(token);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}
void setup()
{

  Serial.begin(115200);
  //  WiFi.disconnect();
  //WiFi.softAPdisconnect(true);
  Serial.println("setting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf("conn...\n");
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  gettoken();

  server.on("/", []() {
    String html = "";
    html += "<html><body>\n";  //此部分为浏览器上显示的信息，可根据实际修改美化
    html += "<h1>esp8266 audio</h1>";
    html += "<p>/play?pm3=mp3url</p>";
    html += "<p>/say?text=hello</p>";
    html += "<p>code by cr4fun</p>"; //关闭
    html += "</body></html>";
    server.send(200, "text/html", html);
  });
  server.on("/play", []() {
    pm3file = server.arg(0);
    const char *url = pm3file.c_str();
    file = new AudioFileSourceICYStream(url);
    buff = new AudioFileSourceBuffer(file, 2048);
    out = new AudioOutputI2SNoDAC();
    mp3 = new AudioGeneratorMP3();
    mp3->begin(buff, out);
    server.send(200, "text/plain", "this works as well");
    while (mp3->isRunning()) {
      if (!mp3->loop()) mp3->stop();
    }
  });

  server.on("/say", []() {
    String text = server.arg(0);
    String url = "http://tsn.baidu.com/text2audio?lan=zh&ctp=1&cuid=abcdxxx&tok=";
    url.concat(token);
    url.concat("&tex=%20%20");
    url.concat(text);
    url.concat("%20%20%20%20%20%20");
    url.concat("&vol=15&per=0&spd=5&pit=5&aue=3");
    Serial.println(url);
    const char *u = url.c_str();
    file = new AudioFileSourceICYStream(u);
    buff = new AudioFileSourceBuffer(file, 2048);
    out = new AudioOutputI2SNoDAC();
    mp3 = new AudioGeneratorMP3();
    mp3->begin(buff, out);
    server.send(200, "text/plain", "this works as well");
    while (mp3->isRunning()) {
      if (!mp3->loop()) {
        delay(1000);
        mp3->stop();
      };
    }
  });
  server.begin();
}


void loop()
{
  server.handleClient();
}
