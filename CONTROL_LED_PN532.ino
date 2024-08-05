#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

const String targetUid = "90 72 7E 20"; // UID của thẻ cụ thể
const int ledPin = 2; // Chân kết nối với đèn LED
bool ledState = false; // Biến theo dõi trạng thái của đèn LED

void setup(void) 
{
  Serial.begin(115200);
  Serial.println("Hệ thống khởi động");
  nfc.begin();
  pinMode(ledPin, OUTPUT); // Khởi tạo chân đèn LED như một đầu ra
  digitalWrite(ledPin, LOW); // Đảm bảo đèn LED ban đầu tắt
}

void loop() 
{
  readNFC();
}

void readNFC() 
{
  if (nfc.tagPresent())
  {
    Serial.println("Thẻ NFC được phát hiện.");
    NfcTag tag = nfc.read();
    String tagId = tag.getUidString();
    tag.print();
    
    if (tagId.equals(targetUid)) 
    {
      Serial.println("UID khớp với thẻ mục tiêu.");
      ledState = !ledState; // Chuyển đổi trạng thái của đèn LED
      digitalWrite(ledPin, ledState ? 1 : 0); // Bật/tắt đèn LED dựa trên trạng thái
      
      // In trạng thái LED và ledState ra Serial Monitor
      Serial.print("Trạng thái ledState: ");
      Serial.println(ledState ? "true" : "false");
      Serial.print("Đèn LED đang: ");
      Serial.println(ledState ? "sáng" : "tắt");
    } else {
      Serial.print("UID không khớp. UID đọc được: ");
      Serial.println(tagId);
    }
  } else {
    Serial.println("Không phát hiện thẻ NFC.");
  }
  delay(5000); // Đợi 5 giây trước khi kiểm tra lại
}
