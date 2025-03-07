
const int wet = 203;
const int dry = 456;

void setup()
{
  Serial.begin(9600);
}

// main loop
void loop()
{
  int soilSensorVal = analogRead(A0);
  int percentageHumidity = map(soilSensorVal, wet, dry, 100, 0);
  Serial.print(percentageHumidity);
  Serial.println("%");
  delay(1000);
}
