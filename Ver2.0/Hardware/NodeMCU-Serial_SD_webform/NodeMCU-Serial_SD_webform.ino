#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include "index.h"

#define BAUD_RATE         115200
#define BPM_MAX_RX_CHAR   32
#define ECG_MAX_RX_CHAR   35

SoftwareSerial swSer(0, 2, false, 300);
File myFile;    /* Creating an File Object */
Sd2Card card;   /* Creating an SD card object */
ESP8266WebServer server(80); //Server on port 80
WiFiClient wifiClient;       // Initialise the WiFi

const int SD_CS = D8; /* SD card CS/SS Pin */
const char* ssid = "Startrek";          // WiFi SSID
const char* password = "Face fade12#";  // WiFi Password

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("\nHardware Serial Started in NodeMCU");

  swSer.begin(BAUD_RATE);
  swSer.println("Software Serial Started in NodeMCU");

  ConnectToWiFi();
  SDcardSetup();  /* Initialize SD card*/

  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/action_page", handleForm); //form action is handled here
  server.on("/registration", handleRegistration); //form action is handled here
  server.on("/upload_data", upload_data);
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();          //Handle client requests
  
  String incomingString = "";
  char count = 0;
  String Unique_ID = server.arg("unique_id");
  while (swSer.available() > 0) {
    
    incomingString = swSer.readStringUntil('#');
  
    if(incomingString.startsWith("$")){
      String bpmString = "";
      
      bpmString = bpmString + incomingString;
      bpmString.remove(0,1);
      bpmString = Unique_ID + "," + bpmString;
      writeToSDcard_BPM(bpmString);
//      Serial.println(bpmString);
    }
    if(incomingString.startsWith("~")){
      String ecgString = "";
      ecgString = ecgString + incomingString;
      ecgString.remove(0,1);
      ecgString = Unique_ID + "," + ecgString;
      writeToSDcard_ECG(ecgString);
      Serial.println(ecgString);
    }
    if(incomingString.length()>40){
      Serial.println("Buffer Overflow");
      break;
    }
  } // while loop ends
} // void loop ends

void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

/*  server.on("/registration", handleRegistration); */
void handleRegistration(){
  String s = Registration_PAGE; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

/*  server.on("/action_page", handleForm);  */
void handleForm() {
  String PersonDetails = "";
  
  String Unique_ID = server.arg("unique_id");
  String firstName = server.arg("firstname");
  String lastName = server.arg("lastname");
  String gender = server.arg("gender");
  String age = server.arg("age");

  PersonDetails = PersonDetails + Unique_ID + "," + firstName + "," + lastName + "," + gender + "," + age;
  Serial.print("Personal Details: ");
  Serial.println(PersonDetails);
  writeToSDcard_Registration(PersonDetails);
  
  String s = "<h2>Form Submitted<h2><h4><a href='/'><button> New Form </button> </a></h4>";
  server.send(200, "text/html", s); //Send web page
}

void upload_data(){
  
  String Unique_ID = server.arg("unique_id");
  
  Serial.print("Uploded UID : ");
  Serial.println(Unique_ID);
  String s = "<h2>Data Uploaded<h2><h4><a href='/'><button> New Upload </button> </a></h4>";
  server.send(200, "text/html", s); //Send web page
  
}

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
    else {  Serial.println("error opening BPM_log.txt");  }
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
    else {  Serial.println("error opening ECG_log.txt");  }
  }
}
/*  writeToSDcard ends  */

void writeToSDcard_Registration(String logData){
  /* Checking for SD Status */
  /* If SD card Status has error then fuction returns Else Opens file and Writes data to it */
  Serial.println("Inside Function");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the SD_CS pin to match your shield or module?");
    return;
  }
  else {
    Serial.println("Inside else condition");
    File dataFileObject = SD.open("Registrations.txt", FILE_WRITE); /* Open file in Write mode */
    Serial.println("File object created");
    Serial.print("Data to write: ");
    Serial.println(logData);
    if (dataFileObject) {               /* Check for File Else print Error */
      Serial.println("Inside if condition");
      Serial.print("Writing to SD card: ");
      dataFileObject.println(logData);  /* Writing data to the file */
      dataFileObject.close();           /* Closing file */
      Serial.println("File closed");
      Serial.println(logData);
    }
    else {  Serial.println("error opening Registration_log.txt");  }
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

void ConnectToWiFi(void) {
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println("WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}
