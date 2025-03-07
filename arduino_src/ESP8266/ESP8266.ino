#include <ESP8266WiFi.h>

const char* wifi_ssid = "SJSU_device";

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

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.SSID());
}

void loop() {
}
