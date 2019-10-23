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

const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.

extern volatile unsigned long timer0_overflow_count;
float fanalog0;
int analog0;

unsigned long BPM_previousMillis = 0;        
const long BPM_interval = 1000;           

unsigned long ECG_previousMillis = 0;     
const long ECG_interval = 1000;           

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
    txString = ReadBPMsensor();
    WriteToSerial(txString);
  }
  
 /*------------  ECG -------------------*/ 
  if(ecgState == HIGH && ecgFlag == HIGH){
      ecgFlag = LOW;
      Serial.println("Initiated ECG Sensing");
      digitalWrite(LED_PIN, HIGH );
      String ecgString = "";
      static int ECGcount=0;
      ECGsensor_flag = true;
      while(ECGsensor_flag && ECGcount < 10){
        unsigned long ECG_currentMillis = millis();
        if (ECG_currentMillis - ECG_previousMillis >= ECG_interval) {
          ECG_previousMillis = ECG_currentMillis;
          ECGcount++;
        }
        fanalog0 = getECG();
        ecgString = ecgString + "~" + rtc.getUnixTime(rtc.getTime()) + "," + String(fanalog0,5)+"#";
        WriteToSerial(ecgString);  
      }
      ECGsensor_flag = false;
      ECGcount = 0;
      Serial.println("Finished ECG Sensing");
  }

} // void loop ends here

void WriteToSerial(String TxString){
//  char tempBuffer[100];
//  TxString.toCharArray(tempBuffer,TxString.length());
  Serial.println(TxString);
  mySerial.print(TxString);
  delay(20);
}

String ReadBPMsensor(void){
  String bpmString = "";
  int myBPM = 0;
  static int BPMcount=0;
  unsigned int mean = 0, sum=0, count=0;
  
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
      sum = sum + myBPM;
      count++;
    }
    delay(20);                    // considered best practice in a simple sketch.
  } // while loop ends here
  mean = sum/count;
  BPMsensor_flag = false;
  BPMcount = 0;
  bpmString = bpmString + "$" + rtc.getUnixTime(rtc.getTime()) + "," + String(mean) + "#";
  
  return bpmString;
} // Function ends here

float getECG(void){
  Serial.println("Reading ECG");
  float analog0;
    // Read from analogic in. 
    analog0=analogRead(2);
    // binary to voltage conversion
    return analog0 = (float)analog0 * 5 / 1023.0; 
}
