#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <dht.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

IPAddress local_IP(192,168,0,150);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

const int led = LED_BUILTIN;  // Built-in led for visual feedback that the button has been pressed
const int button = D0;        // Connect button to pin D0
const int DHTpin = D1;        // DHT sensor to D1

const char* ssid = "Startrek";          // WiFi SSID
const char* password = "Face fade12#";  // WiFi Password

const char* mqtt_server = "192.168.0.123";  // MQTT IP Address
const char* mqtt_topic = "DHT";    // MQTT Topic Name
const char* clientID = "ESP";               // The client id identifies the ESP8266 device.

unsigned long previousMillis_Sensors = 0;        // will store last time LED was updated
const long Sensor_interval = 3000;           // interval at which to blink (milliseconds)

unsigned long previousMillis_SendData = 0;        // will store last time LED was updated
const long MQTT_SendData_interval = 15000;           // interval at which to blink (milliseconds)

String payload="";
char payloadArray[100];
unsigned int payloadIndex=0;

Bounce bouncer = Bounce();                            // Initialise the Pushbutton Bouncer object
dht DHT;                                              // Initialise DHT object 
WiFiClient wifiClient;                                // Initialise the WiFi and MQTT Client objects
PubSubClient client(mqtt_server, 1883, wifiClient);   // 1883 is the listener port for the Broker

void setup() {
  
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(led, HIGH); // Switch the on-board LED off to start with

  bouncer.attach(button);  // Setup pushbutton Bouncer object
  bouncer.interval(5);
  
  Serial.begin(115200);
  ConnectToWiFi();

  if (client.connect(clientID)){                  // Connect to MQTT Broker
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }  
} // void setup ends here

void loop() {
  
  String DHTpayload = "";
  
  unsigned long currentMillis_Sensors = millis();
  if (currentMillis_Sensors - previousMillis_Sensors >= Sensor_interval) {
    previousMillis_Sensors = currentMillis_Sensors;
    DHTpayload = ReadSensor();
  }
  
  unsigned long currentMillis_SendData = millis();
  if (currentMillis_SendData - previousMillis_SendData >= MQTT_SendData_interval) {
    previousMillis_SendData = currentMillis_SendData;
    SendData(DHTpayload);
  }

  bouncer.update();   // Update button state
  if (bouncer.rose()) {
    digitalWrite(led, LOW);
  }
  else if (bouncer.fell()) {
    // Turn light on when button is released
    digitalWrite(led, HIGH);
  }
  
} // void loop ends here

void SendData(String DHTpayload){

  payload = DHTpayload;
  
  if (client.publish(mqtt_topic, payload.c_str())) {      // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
      Serial.println("DHT Payload sent!");  
  }
  //client.publish will return a boolean value depending on whether it succeded or not.
  else {         
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID);
    delay(10);                                              // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(mqtt_topic, payload.c_str());
  }
  payload = "";
  
} // send data ends here

String ReadSensor(void){
  unsigned int curr_Temperature;
  unsigned int curr_Humidity;
  String DHTpayload = "";
  
  DHT.read11(DHTpin);
  curr_Temperature = DHT.temperature;
  curr_Humidity = DHT.humidity;
  Serial.print("Temperature = ");
  Serial.print(curr_Temperature);
  Serial.println(" *C");
  Serial.print("Humidity = ");
  Serial.print(curr_Humidity);
  Serial.println(" %");
  DHTpayload = String(curr_Temperature) + "," + String(curr_Humidity); 

  return DHTpayload;
}

void ConnectToWiFi(void){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //Configuring the WI-FI with the specified static IP.
  WiFi.config(local_IP, gateway, subnet);
 
  //Start the WI-FI connection with specified ACCESS-POINT 
  WiFi.begin(ssid, password);


  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
