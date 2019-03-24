#include <Wire.h>

#define I2C_Request_interval  3000

unsigned long I2C_Request_previousMillis = 0;       

void setup() {
  Serial.begin(115200); /* begin serial for debug */
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
}

void loop() {
  unsigned long I2C_Request_currentMillis = millis();
  if (I2C_Request_currentMillis - I2C_Request_previousMillis >= I2C_Request_interval) {
    I2C_Request_previousMillis = I2C_Request_currentMillis;
    I2C_Request_Data();
    Serial.println();
    //delay(3000);
  }
  //Serial.println();
}

void I2C_Request_Data(void){
  Wire.requestFrom(8, 32); /* request & read data of size 32 from slave */
  while(Wire.available()){
    char c = Wire.read();
    Serial.print(c);
  }
}
