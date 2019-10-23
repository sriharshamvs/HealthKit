#include <SoftwareSerial.h>
#include <Bounce2.h>
#include <Wire.h>
#include <DS3231.h>

#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.

#define RTC_SDA 9  /* Assign Software I2C pin SDA for RTC */
#define RTC_SCL 10  /* Assign Software I2C pin SCL for RTC */


const int BPM_btn = 7;
const int ECG_btn = 8;
const int LED_PIN = LED_BUILTIN;
const int UPLOAD_LED = 2;

const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.

unsigned long BPM_previousMillis = 0;        // will store last time LED was updated
const long BPM_interval = 1000;           // interval at which to blink (milliseconds)

boolean bpmFlag = false;
boolean ecgFlag = false;
boolean bpmState = false;
boolean ecgState = false;
boolean BPMsensor_flag = false;
boolean ECGsensor_flag = false;

SoftwareSerial mySerial(13, 12); // RX, TX
Bounce BPM_debouncer = Bounce();
Bounce ECG_debouncer = Bounce();
DS3231  rtc(RTC_SDA, RTC_SCL);  /* Creating an RTC Object with Software I2C */
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

void setup() {

  pinMode(ECG_btn,INPUT_PULLUP);
  ECG_debouncer.attach(ECG_btn);
  ECG_debouncer.interval(5); // interval in ms

  pinMode(BPM_btn,INPUT_PULLUP);
  BPM_debouncer.attach(BPM_btn);
  BPM_debouncer.interval(5); // interval in ms

  pinMode(LED_PIN,OUTPUT);
  pinMode(UPLOAD_LED,OUTPUT);
  digitalWrite(UPLOAD_LED,LOW);

  Serial.begin(115200);
  while (!Serial);
  Serial.println("Hardware Serial Started in UNO");
  mySerial.begin(115200);
  mySerial.println("Software Serial Started in UNO");
  rtc.begin();                  /* Initialize RTC */
  
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);

} // void setup ends here

void loop() {
  BPM_debouncer.update();
  ECG_debouncer.update();

  bpmState = BPM_debouncer.read();
  ecgState = ECG_debouncer.read();
/*  Check the Status of the Button  */  
 if(bpmState == LOW ){
    bpmFlag = HIGH;
  }
  else{
   digitalWrite(LED_PIN, LOW );        
  }
  if ( ecgState == LOW ) {
     ecgFlag = HIGH;  
  }
  else{
    digitalWrite(LED_PIN, LOW );        
  }
  
/*  Check the Status of the Button along with flag for Error Correction */  
  if(bpmState == HIGH && bpmFlag == HIGH ){
    String txString;
    bpmFlag = LOW;
    BPMsensor_flag = true;
    digitalWrite(LED_PIN, HIGH );
    digitalWrite(UPLOAD_LED,HIGH);
    txString = ReadBPMsensor();
    WriteToSerial(txString);
    digitalWrite(UPLOAD_LED,LOW);
  }
  
  if(ecgState == HIGH && ecgFlag == HIGH){
      ecgFlag = LOW;
      digitalWrite(LED_PIN, HIGH );
      digitalWrite(UPLOAD_LED,HIGH);
      String ecgString = "";
      ecgString = ecgString + "~" + rtc.getUnixTime(rtc.getTime()) + "#";
      WriteToSerial(ecgString);
      digitalWrite(UPLOAD_LED,LOW); 
  }

} // void loop ends here

void WriteToSerial(String TxString){
//  char tempBuffer[100];
//  TxString.toCharArray(tempBuffer,TxString.length());
  Serial.println(TxString);
  mySerial.print(TxString);
}

String ReadBPMsensor(void){
  String bpmString = "";
  int myBPM = 0;
  static int BPMcount=0;
  if (pulseSensor.begin()) {
      Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  while(BPMsensor_flag && BPMcount<10){
    unsigned long BPM_currentMillis = millis();
    if (BPM_currentMillis - BPM_previousMillis >= BPM_interval) {
      BPM_previousMillis = BPM_currentMillis;
      BPMcount++;
    }
    myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
    if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened". 
      Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
      Serial.print("BPM: ");                        // Print phrase "BPM: " 
      Serial.println(myBPM);                        // Print the value inside of myBPM. 
    }
    delay(20);                    // considered best practice in a simple sketch.
  } // while loop ends here
  BPMsensor_flag = false;
  BPMcount = 0;
  bpmString = bpmString + "$" + rtc.getUnixTime(rtc.getTime()) + "," + String(myBPM) +  "#";
  
  return bpmString;
} // Function ends here

  
