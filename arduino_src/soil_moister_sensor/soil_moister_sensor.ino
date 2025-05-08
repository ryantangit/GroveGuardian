#include <ArduinoJson.h>

const int wet = 190;
const int dry = 475;
String message = "";
bool messageReady = false;

const int LED_YELLOW = 4;
const int LED_GREEN = 5;
const int LED_BLUE = 6;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

// main loop
void loop()
{
  while(Serial.available()) {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(1000);  
    message = Serial.readString();
      messageReady = true;
  }
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      messageReady = false;
      return;
    }
    if(doc["type"] == "request") {
      doc["type"] = "response";
      // Get data from analog sensors
      int soilSensorVal = analogRead(A0);
      int percentageHumidity = map(soilSensorVal, wet, dry, 100, 0);
      if (percentageHumidity < 30) {
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, LOW);
      }
      if (percentageHumidity >= 30 && percentageHumidity < 70) {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, LOW);
      }
      if (percentageHumidity > 70) {
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, LOW);
      }
      doc["humidity"] = String(percentageHumidity);
      serializeJson(doc,Serial);
      Serial.println();
    }
    messageReady = false;
  }
}
