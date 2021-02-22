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

const char* host = "192.168.4.66";
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
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
   
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

//void loop()
//{
//    delay(5000);
//    ++value;
//
//    Serial.print("connecting to ");
//    Serial.println(host);
//
//    // Use WiFiClient class to create TCP connections
//    WiFiClient client;
//    const int httpPort = 3000;
//    if (!client.connect(host, httpPort)) {
//        Serial.println("connection failed");
//        return;
//    }
//
//    // We now create a URI for the request
//    String url = "/rssi";
//    //url += "/test";
//
//    String queryString = String("?room=test&rssi=") + String(-77);
//
//    String request = String("POST ") + url + " HTTP/1.1\r\n" +
//                 "Host: " + host + "\r\n" +
//                 "Connection: close\r\n" + 
//                 "Content-Type: application/x-www-form-urlencoded\r\n" +
//                 "\r\n" + queryString + "\r\n" +
//                 "\r\n";
//
//    Serial.print("Request: ");
//    Serial.println(request);
//    client.print(request);
//    // This will send the request to the server
////    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
////                 "Host: " + host + "\r\n" +
////                 "Connection: close\r\n\r\n" + 
////                 queryString + "\r\n");
//    //String queryString = String("{room:test;rssi:") + String(-77) + "}";
//    //client.println(queryString);
//    
//    unsigned long timeout = millis();
//    while (client.available() == 0) {
//        if (millis() - timeout > 5000) {
//            Serial.println(">>> Client Timeout !");
//            client.stop();
//            return;
//        }
//    }
//
//    // Read all the lines of the reply from server and print them to Serial
//    while(client.available()) {
//        String line = client.readStringUntil('\r');
//        Serial.print(line);
//    }
//
//    Serial.println();
//    Serial.println("closing connection\r\n");
//}
