#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_WiFi";  // Tên mạng WiFi bạn muốn phát ra
const char* password = "12345678"; // Mật khẩu của mạng WiFi

IPAddress local_ip(192, 168, 49, 15);
IPAddress gateway(192, 168, 49, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
uint8_t LedPin = 2;
bool LedStatus = LOW;

void event_OnConnect() {
  LedStatus = LOW;
  Serial.println("LED đang tắt");
  server.send(200, "text/html", HTMLGui(LedStatus));
}

void event_ledon() {
  LedStatus = HIGH;
  Serial.println("LED đang bật");
  server.send(200, "text/html", HTMLGui(LedStatus));
}

void event_ledoff() {
  LedStatus = LOW;
  Serial.println("LED đang tắt");
  server.send(200, "text/html", HTMLGui(LedStatus));
}

void event_NotFound() {
  server.send(404, "text/html", "NOT FOUND");
}

void setup() {
  Serial.begin(115200);
  pinMode(LedPin, OUTPUT);
  // Thiết lập ESP32 làm Access Point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", event_OnConnect);
  server.on("/ledon", event_ledon);
  server.on("/ledoff", event_ledoff);
  server.onNotFound(event_NotFound);

  server.begin();
  Serial.println("Server is Starting!");
}

void loop() {
  server.handleClient();
  digitalWrite(LedPin, LedStatus ? HIGH : LOW);
}

String HTMLGui(uint8_t led) {
  String str = "<!DOCTYPE html>\n";
  str += "<html>\n";
  str += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  str += "<title>Document</title>\n";
  str += "<style>html{font-family: Arial, Helvetica, sans-serif;margin: 0px auto;text-align: center;}\n";
  str += "body{margin-top: 50px;} h1{color: white;margin: 50px 30px;} h3{color: white;margin-bottom: 50px;}\n";
  str += ".button{display: block;width: 80px;border: none;color: white;padding: 13px 13px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;border-radius: 4px;}\n";
  str += ".button-off{background-color: blue;}\n";
  str += ".button-off:active{background-color: blueviolet;}\n";
  str += ".button-on{background-color: blue;}\n";
  str += ".button-on:active{background-color: blueviolet;}\n";
  str += ".container{background-color: #00ffff;width: 300px;border-radius: 10px;margin: auto;padding: 10px;}\n";
  str += "p{font-size: 20px;color: bisque;margin-bottom: 10px;}\n";
  str += "</style>\n";
  str += "</head>\n";
  str += "<body>\n";
  str += "    <div class=\"container\">\n";
  str += "        <h1>ESP32 Webserver</h1>\n";
  str += "        <h3>Nguyen Thanh Van</h3>\n";
  if (led) {
    str += "<p>Led Status: ON</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>\n";
  } else {
    str += "<p>Led Status: OFF</p><a class=\"button button-on\" href=\"/ledon\">ON</a>\n";  // Đổi URL thành /ledon khi đèn tắt
  }

  str += "    </div>\n";
  str += "</body>\n";
  str += "</html>\n";
  return str;
}
