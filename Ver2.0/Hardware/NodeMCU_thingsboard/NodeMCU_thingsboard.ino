#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <dht.h>

#define DHT11_PIN D1

const int led = LED_BUILTIN; // This code uses the built-in led for visual feedback that the button has been pressed

const char* ssid     = "Startrek";          // WiFi SSID
const char* password = "Face fade12#";  // WiFi Password
const char* TOKEN    = "wKdNvalcKJwqXPChYHOx";
const char* clientID = "7231e5f0-5e78-11e9-9f0b-c3db1c297b36";
char thingsboardServer[] = "192.168.0.200";

unsigned long lastSend;
unsigned long perviousMillis = 0;
const long ReadSensorIntreval = 5000;

ESP8266WebServer server(80); //Server on port 80
WiFiClient wifiClient;                                // Initialise the WiFi and MQTT Client objects
PubSubClient client (wifiClient);   
dht DHT;

void setup(void) {
  Serial.begin(115200);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); // Switch the on-board LED off to start with
  ConnectToWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
    
} // void setup ends here

void loop(void) {

  if ( !client.connected() ) {
    reconnect();
  }
  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendDHTvalues();
    lastSend = millis();
  }

  client.loop();
  
} // void loop ends here


void getAndSendDHTvalues(void){
  float h;
  float t;
  
  Serial.println("Collecting temperature data.");

  unsigned long currentMillis = millis();
  if(currentMillis - perviousMillis >=  ReadSensorIntreval){
    perviousMillis = currentMillis;
    DHT.read11(DHT11_PIN);
  }
  h = DHT.humidity;  // Reading temperature or humidity takes about 250 milliseconds!
  t = DHT.temperature;  // Read temperature as Celsius (the default)

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  String temperature = String(t);
  String humidity = String(h);


  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); 
  Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":";
  payload += temperature;
  payload += ",";
  payload += "\"humidity\":";
  payload += humidity;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
} // getAndSendDHTvalues ends here

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to Thingsboard node ...");
    if ( client.connect(clientID, TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 2 seconds before retrying
      delay( 2000 );
    }
  }
} // void reconnect ends here
    
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
