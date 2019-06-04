#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoOTA.h>

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>

#include <ESP8266mDNS.h>
#include <FirebaseArduino.h>
//#include <ArduinoJson.h>

PN532_SPI pn532spi(SPI, D3);
PN532 nfc(pn532spi);

#define FIREBASE_HOST "nfc-esp8266.firebaseio.com"
#define FIREBASE_AUTH "7Hn2L1hcmzUduKZMgmAdGYrQJSM9TALaH71JZKtg"
String path = "People_Log";
String person1 = "Mark";
String person2 = "Akhila";
String person3 = "Sarnab";
String person4 = "Yash";

#define WIFI_SSID "EE-IOT-Platform-02"
#define WIFI_PASSWORD "dUQQE?&W44x7"

void setup() {
  // put your setup code here, to run once:
  /*StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["People_Log"] = ""; 
  root["Time"] = 0;*/
  
  Serial.begin(115200);
  Serial.println("Hello! Connect to PN532");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  // connect to wifi.
  Serial.println("Connect to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.set("People_Log", 0);
  Firebase.set(path, 0);
  Firebase.pushString(path, person1);
  Firebase.pushString(path, person2);
  Firebase.pushString(path, person3);
  Firebase.pushString(path, person4);
}

void loop() {

  // put your main code here, to run repeatedly:
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject("{\".sv\":\"timestamp\"}");
  
  uint8_t success;                          // Flag to check if there was an error with the PN532
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t data[20];
  char data_char[20] = {0};
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  Serial.println("Waiting for an ISO14443A Card ...");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      //uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      uint8_t keya[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
      
      // Start with block 4 (the first block of sector 1) since sector 0
      // contains the manufacturer data and it's probably better just
      // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
    
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        //uint8_t data[20];
    
        // If you want to write something to block 4 to test with, uncomment
        // the following line and this text should be read back in a minute
        //memcpy(data, (const uint8_t[]){ 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0 }, sizeof data);
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);
    
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 20);
          Serial.println("");
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    // data[] contains ASCII values; convert to char array to upload to Firebase
    // The first 8 values are the header i.e., data[0] to data[7],
    // the next value, data[8] is the ndefprefix (0x00 = NDEF_URIPREFIX_NONE)
    // the next values are the actual payload, i.e., data[9] is the start of our payload. 0xFE denotes end of payload.
    int i = 9;
    while(data[i] != 0xFE) {
      data_char[i-9] = (char) data[i];
      //Serial.print(data_char[i]);
      i++;
    }
    data_char[i] = '\0';
    // Convert data_char to string
    String str((char*)data_char);
    //Serial.println(str);

    // Now upload to Firebase
    switch(data_char[0]) {
      case 'M': Firebase.set(person1+"/Time", root);
                   if (Firebase.failed()) {
                     Serial.print("Pushing failed:");
                     Serial.println(Firebase.error());  
                     return;
                   }
                   break;
      case 'A': Firebase.set(person2+"/Time", root);
                     if (Firebase.failed()) {
                       Serial.print("Pushing failed:");
                       Serial.println(Firebase.error());  
                       return;
                     }
                     break;
      case 'S': Firebase.set(person3+"/Time", root);
                     if (Firebase.failed()) {
                       Serial.print("Pushing failed:");
                       Serial.println(Firebase.error());  
                       return;
                     }
                     break;
      case 'Y': Firebase.set(person4+"/Time", root);
                   if (Firebase.failed()) {
                     Serial.print("Pushing failed:");
                     Serial.println(Firebase.error());  
                     return;
                   }
                   break;                                                         
      default: Serial.println("Person name not found");
               break;
    }
    /*//Firebase.pushString("People_Log", str);
    Firebase.pushString(path, str);
    // Handle error
    if (Firebase.failed()) {
      Serial.print("Pushing failed:");
      Serial.println(Firebase.error());  
      return;
    }
    Firebase.set(path+"/Time", root);*/
  }  
  // Wait a bit before reading the card again
  delay(5000);
  
}

//https://medium.com/@o.lourme/our-iot-journey-through-esp8266-firebase-angular-and-plotly-js-part-1-a07db495ac5f
//see trouble shooting part if you get pushing failed
