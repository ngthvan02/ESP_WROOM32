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
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

const String targetUid = "90 72 7E 20"; // UID của thẻ cụ thể

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
  if (nfc.tagPresent())
  {
    Serial.println("Thẻ NFC được phát hiện.");
    NfcTag tag = nfc.read();
    String tagId = tag.getUidString();
    tag.print();
    
    if (tagId.equals(targetUid)) 
    {
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
      Serial.print("UID không khớp. UID đọc được: ");
      Serial.println(tagId);
    }
  } else {
    Serial.println("Không phát hiện thẻ NFC.");
  }
  delay(100); // Đợi 5 giây trước khi kiểm tra lại
}

