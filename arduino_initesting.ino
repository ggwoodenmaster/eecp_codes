#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  digitalWrite(2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.begin();
  digitalWrite(2, LOW);
  delay(2155);
}