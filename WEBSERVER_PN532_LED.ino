#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

// Thông tin WiFi và NFC
const char* ssid = "ESP32_WiFi";  // Tên mạng WiFi bạn muốn phát ra
const char* password = "12345678"; // Mật khẩu của mạng WiFi

#define SDA_PIN 21
#define SCL_PIN 22
#define LED_PIN 2 // Chân điều khiển LED

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// UID của thẻ NFC cần kiểm tra
uint8_t targetUID[] = {0x90, 0x72, 0x7E, 0x20};
const int targetUIDLength = sizeof(targetUID);

WebServer server(80);
bool LedStatus = LOW;

void event_OnConnect() {
  server.send(200, "text/html", HTMLGui(LedStatus));
}

void event_led() {
  LedStatus = !LedStatus;
  server.send(200, "text/html", HTMLGui(LedStatus));
}


void event_NotFound() {
  server.send(404, "text/html", "NOT FOUND");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Kết nối WiFi
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(IPAddress(192, 168, 49, 15), IPAddress(192, 168, 49, 1), IPAddress(255, 255, 255, 0));

  // Khởi tạo WebServer
  server.on("/", event_OnConnect);
  server.on("/led", event_led);

  server.onNotFound(event_NotFound);
  server.begin();
  
  Serial.println("Server is Starting!");

  // Kết nối NFC
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  nfc.SAMConfig();
  Serial.println("Waiting for an NFC card ...");
}

void loop() {
  server.handleClient();
  digitalWrite(LED_PIN, LedStatus ? HIGH : LOW);
  
  readNFC();
}

void readNFC() {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500)) {
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println("");

    if (uidLength == targetUIDLength && compareUID(uid, uidLength, targetUID, targetUIDLength)) {
      Serial.println("UID khớp với thẻ mục tiêu.");
      LedStatus = !LedStatus; // Chuyển đổi trạng thái LED
    } else {
      Serial.println("UID không khớp.");
    }
  } else {
    Serial.println("Không phát hiện thẻ NFC.");
  }
  delay(500); // Đợi 0.5 giây trước khi kiểm tra lại
}

// Hàm so sánh UID
bool compareUID(uint8_t *uid1, uint8_t length1, uint8_t *uid2, uint8_t length2) {
  if (length1 != length2) return false;
  for (uint8_t i = 0; i < length1; i++) {
    if (uid1[i] != uid2[i]) return false;
  }
  return true;
}

String HTMLGui(bool led) {
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
    str += "<p>Led Status: ON</p><a class=\"button button-off\" href=\"/led\">OFF</a>\n";
  } else {
    str += "<p>Led Status: OFF</p><a class=\"button button-on\" href=\"/led\">ON</a>\n";  // Đổi URL thành /ledon khi đèn tắt
  }
  str += "    </div>\n";
  str += "</body>\n";
  str += "</html>\n";
  return str;
}
