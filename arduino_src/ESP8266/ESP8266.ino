  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include "WiFiCredential.h"
  #include <ArduinoJson.h>
  
  void setup()
  {
    
    Serial.begin(9600);
    //Serial.println();
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);    
    }
  
  }
  
  void loop() {
  
    if(WiFi.status() != WL_CONNECTED) {
      //Serial.println("Not connected to a WiFi network");
      return;
    }
    WiFiClient wifi_client;
    HTTPClient http;
  
    http.begin(wifi_client, post_ping_url);
    http.addHeader("Content-Type", "application/json");
  
    
    //Sending request
    DynamicJsonDocument doc(1024);
  
    doc["type"] = "request";
    serializeJson(doc,Serial);
    Serial.println();
    boolean messageReady = false;
    String message = "";
    while(messageReady == false) { // blocking but that's ok
      if(Serial.available()) {
        message = Serial.readString();
        messageReady = true;
      }
    }
    DeserializationError error = deserializeJson(doc, message);
    if(error) {
      return;
    }
    
    //Retrieval of information
    StaticJsonDocument<200> body_doc;
    body_doc["message"] = doc["humidity"];
    String requestBody;
    serializeJson(body_doc, requestBody);
    //Debugging Message to send back to arduino
    serializeJson(body_doc, Serial);
    Serial.println();
    // Send POST request
    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
      String response = http.getString();
      //Serial.println("Response: " + response);
    } else {
      //Serial.println("Error on HTTP request");
    }
    delay(10000);
  
  }
