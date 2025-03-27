#include <SoftwareSerial.h>

#define RX 3
#define TX 1
#define BAUD_RATE 9600

EspSoftwareSerial::UART espSerial;
std::atomic<bool> dataReceived(false);

void IRAM_ATTR onReceive() {
  dataReceived.store(true);
  esp_schedule();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  espSerial.begin(BAUD_RATE, EspSoftwareSerial::SWSERIAL_8N1, RX, TX);
  espSerial.onReceive(onReceive);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (dataReceived.exchange(false)) {
    while (espSerial.available()) {
      Serial.write(espSerial.read());
    }
    esp_suspend();
  }
}
