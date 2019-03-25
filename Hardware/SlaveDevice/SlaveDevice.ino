/* 
 * Project       : Healthkit (2019) 
 * Version       : 1.0
 * Author        : Sriharsha MVS
 * Device        : Slave
 *
 * This Program Reads Health parametes from the Sensors and Sends the data to Master Device. 
 * 
 * The following data pins are used on the Arduino UNO
 * ~5, A4, A5, 5V, 3V3 and GND
 * 
 * The I2C communication uses I2C bus pins:
 * Arduino UNO     NodeMCU
 * A4(SDA)    <->  D1 (SDA)
 * A5(SCL)    <->  D2 (SCL)
 * GND        <->  GND
 * 
 * The DHT Sensor Uses PWM pin :
 * Arduino UNO      DHT 
 * ~5         <->   Data
 * 3V3        <->   Vcc
 * GND        <->   GND   
 */
 
#include <Wire.h>
#include <dht.h>
#include <DS3231.h>

#define DHTPIN  5   /* Assign DHT pin */
#define RTC_SDA A1  /* Assign Software I2C pin SDA for RTC */
#define RTC_SCL A0  /* Assign Software I2C pin SCL for RTC */

unsigned long ReadSensors_previousMillis = 0;       
const int ReadSensors_interval = 3000;  /* Reading Sensor Data interval, You can change this value to your choice */

String comString="";  /* Creating an Empty String for Data Appending */
char comBuffer[100];  /* Creating an Empty Char array for Data transmission */

dht DHT;    /* Creating an DHT Object */
DS3231  rtc(RTC_SDA, RTC_SCL);  /* Creating an RTC Object with Software I2C */

void setup() {
  Serial.begin(115200);         /* Begin Serial Communication */ 
  Wire.begin(8);                /* Join I2C bus with Address #8 */
  Wire.onRequest(requestEvent); /* Register Request Event */
  rtc.begin();                  /* Initialize RTC */
}
/* void setup ends  */

void loop() {
  /* Calling Function as per the ReadSensors_interval defined above*/
  unsigned long ReadSensors_currentMillis = millis();
  if (ReadSensors_currentMillis - ReadSensors_previousMillis >= ReadSensors_interval) {
    ReadSensors_previousMillis = ReadSensors_currentMillis;
    ReadSensors_Data(); /* Calling ReadSensors_Data Function */
  }
}
/* void loop ends */

/*  This Function is used to Read the Sensor values along with the Time Stamp and store it to a String Variable */
void ReadSensors_Data(void){
  unsigned char temp=0, humid=0;
  DHT.read11(DHTPIN);       /* Reading DHT Sensor */
  temp = DHT.temperature;
  humid = DHT.humidity;
  /* Appending Temperature & Humidity values, Day, Date, Time Values to the String */
  /* Format  : TEMP,HUMID,DAY,DATE,TIME */
  /* Example : 33,48,Sunday,25/03/2019,10:51:25 */ 
  comString = String(temp) + "," + String(humid);   
  comString = comString + "," + rtc.getDOWStr() + "," + rtc.getDateStr() + "," + rtc.getTimeStr(); 
  comString = comString +'\0';  /* Adding NULL char to the end of the String */  
  Serial.println(comString);
  /* Converting String Variable to Char Array for Data Transmission in I2C Bus */
  comString.toCharArray(comBuffer,comString.length());
}
/*  ReadSensors_Data ends */

/* This Function that executes whenever data is requested from master */
void requestEvent() {
  Wire.write(comBuffer);
}
/*  requestEvent ends */
