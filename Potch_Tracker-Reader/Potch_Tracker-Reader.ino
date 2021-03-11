/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid     = "Rivendell";
const char* password = "lamppost";

const char* host = "192.168.4.67";
const char* port = "3000";

const char* room = "test";

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  //Send an HTTP POST request every 10 minutes
  delay(5000);
  ++value;

  int rssi = random(-99,-30);

  Serial.printf("Round: %d\r\n", value);
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;

    String url = "http://" + String(host) + ":" + String(port) + "/rssi";
    // Your Domain name with URL path or IP address with path
    http.begin(url);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "room=" + String(room) + "&rssi=" + String(rssi);
    Serial.print("Request: ");
    Serial.println(httpRequestData);         
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
   
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  // lastTime = millis();
}
