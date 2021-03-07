/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>
#include <WiFi.h>
#include <HTTPClient.h>

BLEAddress addr = BLEAddress("c3:c0:3a:37:5a:d9");
const char* ssid     = "Rivendell";
const char* password = "lamppost";

const char* host     = "192.168.4.67";
int         port     = 3000;

const char* room     = "test";

byte        mac[6];
String      macStr;

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
WiFiClient client;

String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

void connect_wifi () {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.macAddress(mac);
    macStr = mac2String(mac);
    Serial.print("MAC: ");
    Serial.println(macStr);
  }
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      if (advertisedDevice.getAddress().equals(addr)) {
        int rssi = advertisedDevice.getRSSI();
        //Check WiFi connection status
        Serial.printf("Found %s\n", advertisedDevice.getAddress().toString().c_str());
        if (WiFi.status() == WL_CONNECTED) {

          if (client.connect(host, port)) {
            client.stop();
            HTTPClient http;

            String url = "http://" + String(host) + ":" + String(port) + "/rssi";
            // Your Domain name with URL path or IP address with path
            http.begin(url);

            // Specify content-type header
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");
            // Data to send with HTTP POST
            String httpRequestData = "mac=" + macStr + "&rssi=" + String(rssi);
            Serial.print("Request: ");
            Serial.println(httpRequestData);
            // Send HTTP POST request
            int httpResponseCode = http.POST(httpRequestData);

            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            // Free resources
            http.end();      }
          } else {
            Serial.println("Cannot connect to host.");
            // WiFi is blocking the BLE loop when the host can't be found
            // so kill the WiFi and try again
            client.stop();
            WiFi.disconnect();
          }
        } else {
          Serial.println("WiFi Disconnected");
          connect_wifi();
        }
      }
      // Serial.println("end onResult");
    }
};

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  connect_wifi();
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);
}
