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

bool ledState = false;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Đảm bảo LED tắt khi khởi động

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  nfc.SAMConfig();
  Serial.println("Waiting for an NFC card ...");
}

void loop(void) {
  readNFC();
}

void readNFC() {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Bộ đệm để lưu trữ UID được trả về
  uint8_t uidLength;

  // Đặt thời gian chờ là 1 giây (1000 ms)
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000)) {
    Serial.println("Thẻ NFC được phát hiện.");
    
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // So sánh UID quét được với UID đã thiết lập sẵn
    if (uidLength == targetUIDLength && compareUID(uid, uidLength, targetUID, targetUIDLength)) {
      // Đổi trạng thái LED
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      Serial.print("LED ");
      Serial.println(ledState ? "ON" : "OFF");
    } else {
      Serial.println("UID không khớp.");
    }
  } else {
    Serial.println("Không phát hiện thẻ NFC nào sau 1 giây.");
  }
  delay(2000);
}

// Hàm so sánh UID
bool compareUID(uint8_t *uid1, uint8_t length1, uint8_t *uid2, uint8_t length2) {
  if (length1 != length2) return false;
  for (uint8_t i = 0; i < length1; i++) {
    if (uid1[i] != uid2[i]) return false;
  }
  return true;
}
