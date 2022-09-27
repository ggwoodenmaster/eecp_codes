#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  digitalWrite(2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.begin();
  digitalWrite(2, LOW);
<<<<<<< HEAD
  delay(2055);
=======
  delay(2005);
>>>>>>> 053aac40d76203d026ffaab176fc504dfe572dcd
}