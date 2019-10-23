#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
#include <Bounce2.h> 

#include "index.h"

#define BAUD_RATE         115200
#define BPM_MAX_RX_CHAR   32
#define ECG_MAX_RX_CHAR   35
#define UPLOAD_LED        A0

const int SD_CS = D8; /* SD card CS/SS Pin */
const int led = LED_BUILTIN; // This code uses the built-in led for visual feedback that the button has been pressed
const int button = D0;       // Connect your button to pin #13

const char* ssid = "ravi";          // WiFi SSID
const char* password = "raviteja";  // WiFi Password

const char* mqtt_server = "192.168.43.173";  // MQTT IP Address
const char* mqtt_topic_bpm = "HK/BPM";    // MQTT Topic Name
const char* mqtt_topic_ecg = "HK/ECG";    // MQTT Topic Name
const char* clientID = "ESP8266";     // The client id identifies the ESP8266 device.

String Unique_ID="";

SoftwareSerial swSer(0, 2, false, 300);
File myFile;    /* Creating an File Object */
Sd2Card card;   /* Creating an SD card object */
ESP8266WebServer server(80); //Server on port 80
WiFiClient wifiClient;       // Initialise the WiFi
PubSubClient client(mqtt_server, 1883, wifiClient);   // 1883 is the listener port for the Broker
Bounce bouncer = Bounce();  // Initialise the Pushbutton Bouncer object

void setup() {
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(led, HIGH); // Switch the on-board LED off to start with
  pinMode(UPLOAD_LED,OUTPUT);
  digitalWrite(UPLOAD_LED,LOW);
  bouncer.attach(button);  // Setup pushbutton Bouncer object
  bouncer.interval(5);
  
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
  delay(500);
  if (client.connect(clientID)){                  // Connect to MQTT Broker
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void loop() {
  server.handleClient();          //Handle client requests
  ScanBTNstatus();
  
  String incomingString = "";
  char count = 0;
  Unique_ID = server.arg("unique_id");
  while (swSer.available() > 0) {
    
    incomingString = swSer.readStringUntil('#');
  
    if(incomingString.startsWith("$")){
      String bpmString = "";
      String BPMpayload = "";
      
      bpmString = bpmString + incomingString;
      bpmString.remove(0,1);
      BPMpayload = BPMpayload + "B" + "," +  Unique_ID + "," + bpmString;
      digitalWrite(UPLOAD_LED,HIGH);
      Serial.print("UID: ");
      Serial.println(Unique_ID);
      SendDataToServer_BPM(BPMpayload);
      bpmString = Unique_ID + "," + bpmString;
      writeToSDcard_BPM(bpmString);
      digitalWrite(UPLOAD_LED,LOW);
//      Serial.println(bpmString);
    }
    
    if(incomingString.startsWith("~")){
      String ecgString = "";
      String ECGpayload = "";
      
      ecgString = ecgString + incomingString;
      ecgString.remove(0,1);
      ECGpayload = ECGpayload + "E" + "," +  Unique_ID + "," + ecgString;
      digitalWrite(UPLOAD_LED,HIGH);
      SendDataToServer_ECG(ECGpayload);
      ecgString = Unique_ID + "," + ecgString;
      writeToSDcard_ECG(ecgString);
      digitalWrite(UPLOAD_LED,LOW);
    }
    
    if(incomingString.length()>40){
      Serial.println("Buffer Overflow");
      break;
    }
  } // while loop ends
} // void loop ends

void SendDataToServer_BPM(String HKpayload){
  String payload="";
  char payloadArray[200];
  unsigned int payloadIndex=0;
  
  payload = HKpayload;
  if (client.publish(mqtt_topic_bpm, payload.c_str())) {      // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
      Serial.println("BPM Payload sent!"); 
  }
  //client.publish will return a boolean value depending on whether it succeded or not.
  else {         
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID);
    delay(10);                                              // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(mqtt_topic_bpm, payloadArray);
  }
} // send data ends

void SendDataToServer_ECG(String HKpayload){
  String payload="";
  char payloadArray[200];
  unsigned int payloadIndex=0;
  
  payload = HKpayload;
  if (client.publish(mqtt_topic_ecg, payload.c_str())) {      // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
      Serial.println("ECG Payload sent!"); 
  }
  //client.publish will return a boolean value depending on whether it succeded or not.
  else {         
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID);
    delay(10);                                              // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(mqtt_topic_ecg, payloadArray);
  }
} // send data ends

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
  
  Unique_ID = server.arg("unique_id");
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
  
  Unique_ID = server.arg("unique_id");
  
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

void ScanBTNstatus(void){
  bouncer.update();   // Update button state

  if (bouncer.rose()) {
    digitalWrite(led, LOW);

    if (client.publish(mqtt_topic_bpm, "Button pressed!")) {      // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
      Serial.println("Button pushed and message sent!");
    }
  
    else {                                                    //client.publish will return a boolean value depending on whether it succeded or not.
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10);                                              // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(mqtt_topic_bpm, "Button pressed!");
    }
  }
  else if (bouncer.fell()) {
    // Turn light on when button is released
    digitalWrite(led, HIGH);
  }
}

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
