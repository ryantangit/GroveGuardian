#include <ArduinoJson.h>

const int wet = 203;
const int dry = 456;
String message = "";
bool messageReady = false;

void setup()
{
  Serial.begin(9600);
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
      Serial.print(message);
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
      doc["humidity"] = String(percentageHumidity) + "%";
      serializeJson(doc,Serial);
    }
    messageReady = false;
  }
}
