#include <Wire.h>
#include <dht.h>
#include <OneWire.h>
dht DHT;
#define DHT11_PIN 5

unsigned char tempbuff[20];

void setup() {
  Serial.begin(9600);
  Wire.begin(2);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
  DHT.read11(5);
  

  tempbuff[0] = char(DHT.temperature);
 
  tempbuff[1] = char(DHT.humidity);


Serial.print(tempbuff[0]);
Serial.print(".\t");
Serial.print(tempbuff[1]);
Serial.print(".\t");


  delay(2000);
}


void requestEvent() 
{
  int temp = tempbuff[0];
  int hum = tempbuff[1];
  
  Wire.write(temp);
  Wire.write(hum);
 
  
}
