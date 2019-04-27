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
String token = "24.3723a9f93797598f4fb6163e81c52955.2592111.1558893321.282335-15738833";
void setup()
{

  Serial.begin(115200);
  delay(1000);
//  WiFi.disconnect();
  //WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    delay(1000);
  }
  Serial.println(WiFi.localIP());
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
  server.on("/test", []() {
    String value = server.arg(0);
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/say", []() {
    String text = server.arg(0);
    String url = "http://tsn.baidu.com/text2audio?lan=zh&ctp=1&cuid=abcdxxx&tok=";
    url.concat(token);
    url.concat("&tex=%20%20");
    url.concat(text);
    url.concat("%20%20%20%20%20%20");
    url.concat("&vol=9&per=0&spd=5&pit=5&aue=3");
    Serial.println(url);
    const char *u = url.c_str();
    file = new AudioFileSourceICYStream(u);
    buff = new AudioFileSourceBuffer(file, 2048);
    out = new AudioOutputI2SNoDAC();
    mp3 = new AudioGeneratorMP3();
    mp3->begin(buff, out);
    server.send(200, "text/plain", "this works as well");
    while (mp3->isRunning()) {
      if (!mp3->loop()) mp3->stop();
    }
  });
  server.begin();
}


void loop()
{

  server.handleClient();
  if (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    WiFi.begin(SSID, PASSWORD);
  }
}
