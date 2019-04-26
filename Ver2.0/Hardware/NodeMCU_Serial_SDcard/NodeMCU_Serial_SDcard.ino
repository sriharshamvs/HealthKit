#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define BAUD_RATE         115200
#define BPM_MAX_RX_CHAR   32
#define ECG_MAX_RX_CHAR   35

SoftwareSerial swSer(0, 2, false, 300);
File myFile;    /* Creating an File Object */
Sd2Card card;   /* Creating an SD card object */

const int SD_CS = D8; /* SD card CS/SS Pin */

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Hardware Serial Started in NodeMCU");

  swSer.begin(BAUD_RATE);
  swSer.println("Software Serial Started in NodeMCU");

  SDcardSetup();  /* Initialize SD card*/
}

void loop() {
  String incomingString = "";
  char count = 0;
  while (swSer.available() > 0) {
    
    incomingString = swSer.readStringUntil('#');
  
    if(incomingString.startsWith("$")){
      String bpmString = "";
      bpmString = bpmString + "BPM"+ "," + incomingString;
      Serial.println(bpmString);
      bpmString.remove(0,5);
      writeToSDcard_BPM(bpmString);
      Serial.println(bpmString);
    }
    if(incomingString.startsWith("~")){
      String ecgString = "";
      ecgString = ecgString + "ECG" + "," + incomingString;
      Serial.println(ecgString);
      ecgString.remove(0,5);
      writeToSDcard_ECG(ecgString);
      Serial.println(ecgString);
    }
    if(incomingString.length()>40){
      Serial.println("Buffer Overflow");
      break;
    }
  } // while loop ends
} // void loop ends

void writeToSDcard_BPM(String logData){
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
    File dataFile = SD.open("BPM_log.txt", FILE_WRITE); /* Open file in Write mode */
    if (dataFile) {               /* Check for File Else print Error */
      Serial.print("Writing to SD card: ");
      dataFile.println(logData);  /* Writing data to the file */
      dataFile.close();           /* Closing file */
      Serial.println(logData);
    }
    else {  Serial.println("error opening HKlog.txt");  }
  }
}
/*  writeToSDcard ends  */

void writeToSDcard_ECG(String logData){
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
    File dataFile = SD.open("ECG_log.txt", FILE_WRITE); /* Open file in Write mode */
    if (dataFile) {               /* Check for File Else print Error */
      Serial.print("Writing to SD card: ");
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
