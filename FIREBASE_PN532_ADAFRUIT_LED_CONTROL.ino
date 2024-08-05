#include "WiFi.h"
#include "FirebaseESP32.h"

#define WIFI_SSID "Nguyen thanh van"
#define WIFI_PASSWORD "27101997"
#define FIREBASE_AUTH "WO6rQRzrhJclOodRzR6zheECOmY8uCmBINtREXj8"
#define FIREBASE_HOST "test-control-aeb82-default-rtdb.firebaseio.com"

#define LED 2
int load1=0;

FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;
String path = "/";


#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define LED_PIN 2 // Chân điều khiển LED

// Chọn chân I2C phù hợp với ESP32
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// UID của thẻ NFC cần kiểm tra
uint8_t targetUID[] = {0x90, 0x72, 0x7E, 0x20};
const int targetUIDLength = sizeof(targetUID);

void initFirebase() {
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
  Serial.println("Kết nối Firebase thành công!!");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200);
  Serial.println();
  // Kết nối WiFi
  Serial.print("Connecting to WiFi:");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Kết nối Wifi thành công!!!");
  initFirebase(); // Kêt nối Firebase
  nfc.begin();
  pinMode(LED, OUTPUT); digitalWrite(LED, LOW);  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  nfc.SAMConfig();
  Serial.println("Waiting for an NFC card ...");
}

void loop() {
  // put your main code here, to run repeatedly:
  // set data
readNFC();

if(Firebase.get(firebaseData,"/esp32demo/L1")) {
 if (firebaseData.dataType() == "string") {
 load1 = firebaseData.stringData().toInt();
       


 Serial.print("Đèn LED đang: ");
 Serial.println(load1 ? "sáng" : "tắt");
 }
}
digitalWrite(LED, load1);   
}

void readNFC() 
{
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Bộ đệm để lưu trữ UID được trả về
  uint8_t uidLength;

   // Đặt thời gian chờ là 1 giây (1000 ms)
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500)) {
  
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println("");
    
    if (uidLength == targetUIDLength && compareUID(uid, uidLength, targetUID, targetUIDLength)) {
    
      Serial.println("UID khớp với thẻ mục tiêu.");
      load1 = !load1; // Chuyển đổi trạng thái của đèn LED
      if(Firebase.setString(firebaseData, "/esp32demo/L1", String(load1))){
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.println("------------------------------------");
      Serial.println();
      Serial.println("set: " + String(load1));
    }else{
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }

    } else {
      Serial.print("UID không khớp. ");
      
    }
  } else {
    Serial.println("Không phát hiện thẻ NFC.");
  }
  delay(500); // Đợi 5 giây trước khi kiểm tra lại
}
// Hàm so sánh UID
bool compareUID(uint8_t *uid1, uint8_t length1, uint8_t *uid2, uint8_t length2) {
  if (length1 != length2) return false;
  for (uint8_t i = 0; i < length1; i++) {
    if (uid1[i] != uid2[i]) return false;
  }
  return true;
}

