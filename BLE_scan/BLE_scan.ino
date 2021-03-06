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

const char* host = "192.168.4.67";
const char* port = "3000";

const char* room = "test";

int scanTime = 5; //In seconds
BLEScan* pBLEScan;


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.getAddress().equals(addr)) {
        //Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED) {
          /* HTTPClient http;

          int rssi = advertisedDevice.getRSSI();

          String url = "http://" + String(host) + ":" + String(port) + "/rssi";
          // Your Domain name with URL path or IP address with path
          http.begin(url);

          // Specify content-type header
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          // Data to send with HTTP POST
          String httpRequestData = "room=" + String(room) + "&rssi=" + String(rssi);
          // Serial.print("Request: ");
          // Serial.println(httpRequestData);
          // Send HTTP POST request
          int httpResponseCode = http.POST(httpRequestData);

          // Serial.print("HTTP Response code: ");
          // Serial.println(httpResponseCode);

          // Free resources
          http.end(); */
        }
        else {
          // Serial.println("WiFi Disconnected");
        }
        // lastTime = millis();
      }
    }
};

void setup() {
  /* Serial.begin(115200);
  delay(10);
  Serial.println("Scanning...");
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid); */

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    /* Serial.print(".");

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); */
  }

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value


}

void loop() {
  // put your main code here, to run repeatedly:

  delay(5000);

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  // Serial.print("Devices found: ");
  // Serial.println(foundDevices.getCount());
  // Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

}
