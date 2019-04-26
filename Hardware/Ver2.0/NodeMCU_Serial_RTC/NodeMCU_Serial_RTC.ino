#include <SoftwareSerial.h>

#define BAUD_RATE 115200

SoftwareSerial swSer(0, 2, false, 300);

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
    
    incomingString = swSer.readStringUntil('#');
  
    if(incomingString.startsWith("$")){
      String bpmString = "";
      bpmString = bpmString + "BPM"+ "," + incomingString;
      Serial.println(bpmString);
      bpmString.remove(0,5);
      Serial.println(bpmString);
    }
    if(incomingString.startsWith("~")){
      String ecgString = "";
      ecgString = ecgString + "ECG" + "," + incomingString;
      Serial.println(ecgString);
      ecgString.remove(0,5);
      Serial.println(ecgString);
    }
    if(incomingString.length()>40){
      Serial.println("Buffer Overflow");
      break;
    }
  }
} // void loop ends here
