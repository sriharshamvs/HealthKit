/* 
 * Project       : Healthkit (2019) 
 * Version       : 1.0
 * Author        : Sriharsha MVS
 * Device        : Master
 *
 * This Program recieves Health parametes from the Slave device and logs the Data to a file. 
 * 
 * The following data pins are used on the NodeMCU 1.0(ESP-12E Module)
 * D1, D2, D5, D6, D7, D8, 3V3 and GND
 * 
 * The I2C communication uses I2C bus pins:
 * NodeMCU       Arduino UNO
 * D1(SDA)  <->  A4 (SDA)
 * D2(SCL)  <->  A5 (SCL)
 * GND      <->  GND
 * 
 * The SD-Card Reader uses the SPI bus pins:
 * NodeMCU      SD-CARD Reader
 * D5     <->   SCK/CLK
 * D6     <->   MISO
 * D7     <->   MOSI
 * D8     <->   CS
 * Vcc    <->   Vcc
 * GND    <->   GND   
 */

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

const int SD_CS = D8; /* SD card CS/SS Pin */

unsigned long I2C_Request_previousMillis = 0;       
const int I2C_Request_interval = 3000;  /* I2C Request Data interval, You can change this value to your choice */

File myFile;    /* Creating an File Object */ 
Sd2Card card;   /* Creating an SD card object */ 

void setup() {
  Serial.begin(115200); /* Begin Serial Communication */
  Wire.begin(D1, D2); /* Join I2C SDA=D1 & SCL=D2 */
  SDcardSetup();  /* Initialize SD card*/
}
/* void setup ends  */
void loop() {
  /* Calling Function as per the I2C_Request_interval defined above*/
  unsigned long I2C_Request_currentMillis = millis();
  if (I2C_Request_currentMillis - I2C_Request_previousMillis >= I2C_Request_interval) {
    I2C_Request_previousMillis = I2C_Request_currentMillis;
    I2C_Request_Data(); /* Calling I2C_Request_Data Function */
  }
}
/* void loop ends */

/* This Function is used to Request Data from the Slave Device in an I2C Bus and also contain "writeToSDcard" function */
void I2C_Request_Data(void){
  String dataString = "";   /* Creating an empty String */
  
  Wire.requestFrom(8, 32);  /* Request & Read data of size 32 bytes from slave */
  while(Wire.available()){
    char c = Wire.read();   /* Reading Incoming data and storing to a byte size/character size variable */ 
    //Serial.print(c);
    dataString = dataString + String(c);  /* Appending the incoming data to the String */
  }
  writeToSDcard(dataString);  /* Calling writeToSDcard function with dataString as parameter */
  Serial.println();
}
/*  I2C_Request_Data ends */

/* This Function is used Write The Data to an SD card by accepting "logData" as an parameter */
void writeToSDcard(String logData){
  /* Checking for SD Status */
  /* If SD card Status has error then fuction returns Else Opens file and Writes data to it */
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the SD_CS pin to match your shield or module?");
    return; 
  }
  else {  
    File dataFile = SD.open("HKlog.txt", FILE_WRITE); /* Open file in Write mode */
    if (dataFile) {               /* Check for File Else print Error */
      dataFile.println(logData);  /* Writing data to the file */
      dataFile.close();           /* Closing file */
      Serial.println(logData);
    }
    else {  Serial.println("error opening HKlog.txt");  }
  }
}
/*  writeToSDcard ends  */

/* This Function is used to initialize the SD card */
void SDcardSetup(void){
 Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");
}
/*  SDcardSetup ends  */
