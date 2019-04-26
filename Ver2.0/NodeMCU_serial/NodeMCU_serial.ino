
#include <SoftwareSerial.h>

#define BAUD_RATE 115200

SoftwareSerial swSer(0, 4, false, 300);

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Hardware Serial Started in NodeMCU");
  
  swSer.begin(BAUD_RATE);
  swSer.println("Software Serial Started in NodeMCU");
  
}

void loop() {
  String incomingString = "";
  char incomingByte;
  char count = 0;
  while (swSer.available() > 0) {
    incomingByte = swSer.read();
    if((incomingByte == '~' || incomingByte == '$') && count++ < 10){
      incomingString = incomingString + String(incomingByte);  
    }
    if(count >10){
      break;0
    }
    Serial.print(incomingString);
    yield();
  }
  //Serial.println(incomingString);
  delay(500);
} // void loop ends here
