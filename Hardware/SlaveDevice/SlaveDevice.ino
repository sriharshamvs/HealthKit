#include <Wire.h>
#include <dht.h>
#include <DS3231.h>

#define DHTPIN  5
#define RTC_SDA A1
#define RTC_SCL A0

dht DHT;
DS3231  rtc(RTC_SDA, RTC_SCL);

unsigned char temp=0, humid=0;
String comString="";
char comBuffer[20];

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
//  Wire.onReceive(receiveEvent); // register receive event
  Wire.onRequest(requestEvent); // register request event
  rtc.begin();
  Serial.begin(115200);           // start serial for debug
}

void loop() {
  DHT.read11(DHTPIN);
  temp = DHT.temperature;
  humid = DHT.humidity;
  comString = String(temp) + "," + String(humid);
  //comString = comString + String(rtc.getDOWStr()) + String(rtc.getDateStr()) + String(rtc.getTimeStr());
  comString = comString + "," + rtc.getDOWStr() + "," + rtc.getDateStr() + "," + rtc.getTimeStr();
  comString = comString +'\0';
//  Serial.print("Temperature = ");
//  Serial.print(DHT.temperature);
//  Serial.print(" *C \t");
//  Serial.print("Humidity = ");
//  Serial.print(DHT.humidity);
//  Serial.println(" %");
    Serial.println(comString);
    //DisplayTime();
    comString.toCharArray(comBuffer,comString.length());
    
  delay(3000);
}

// function that executes whenever data is received from master
//void receiveEvent(int howMany) {
//  while (0 < Wire.available()) {
//    char c = Wire.read(); // receive byte as a character
//    Serial.print(c);         // print the character
//  }
//  Serial.println();         // to newline
//}

// function that executes whenever data is requested from master
void requestEvent() {
  //Wire.write("Hello NodeMCU");//send string on request
  Wire.write(comBuffer);
}

/*void DisplayTime(){
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  
  // Wait one second before repeating :)
  delay (1000);
}*/
