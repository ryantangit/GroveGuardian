#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* wifi_ssid = "REPLACE";
const char* wifi_password = "REPLACE HERE";
const String get_ping_url = "http://IP/ping"; //

void setup()
{
  Serial.begin(9600);
  Serial.println();
  WiFi.begin(wifi_ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");    
  }
  Serial.println();

  Serial.println("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.SSID());
  Serial.println();
}

void loop() {

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to a WiFi network");
    return;
  }

  WiFiClient wifi_client;
  HTTPClient http;

  http.begin(wifi_client, get_ping_url);

  int http_response_code = http.GET();
  if (http_response_code <= 0) {
    Serial.print("HTTP Request to ping failed");
    Serial.println(http_response_code);
  } 
  
  Serial.println("HTTP Response code: ");
  Serial.println(http_response_code);
  String response = http.getString();
  Serial.println("Response body: ");
  Serial.println(response);
  http.end();

  
  delay(5000);
}
