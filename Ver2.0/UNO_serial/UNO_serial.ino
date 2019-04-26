#include <SoftwareSerial.h>
#include <Bounce2.h>

const int BPM_btn = 7;
const int ECG_btn = 6;
const int LED_PIN = LED_BUILTIN;

boolean bpmFlag = false;
boolean ecgFlag = false;
boolean bpmState = false;
boolean ecgState = false;

SoftwareSerial mySerial(10, 11); // RX, TX
Bounce BPM_debouncer = Bounce();
Bounce ECG_debouncer = Bounce();

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
} // void setup ends here

void loop() {
  BPM_debouncer.update();
  ECG_debouncer.update();

  bpmState = BPM_debouncer.read();
  ecgState = ECG_debouncer.read();
  
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
  
  if(bpmState == HIGH && bpmFlag == HIGH ){
      bpmFlag = LOW;
      digitalWrite(LED_PIN, HIGH );
      mySerial.write("$");
  }
  if(ecgState == HIGH && ecgFlag == HIGH){
      ecgFlag = LOW;
      digitalWrite(LED_PIN, HIGH );
      mySerial.write("~"); 
  }

//  while (mySerial.available()) {
//    char incomingByte = mySerial.read();
//    Serial.write(incomingByte);
//  }
//  if (Serial.available()) {
//    mySerial.write(Serial.read());
//  }
} // void loop ends here
