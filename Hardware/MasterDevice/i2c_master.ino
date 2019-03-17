#include <Wire.h>

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  Wire.requestFrom(2, 2);   
  while (Wire.available()) 
  { 
    int a = Wire.read(); 
 int b = Wire.read(); 
    Serial.println(a);// print the character
   Serial.println(b);
  }

  delay(2000);
}
