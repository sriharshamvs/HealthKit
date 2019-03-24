#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

const int led = LED_BUILTIN; // This code uses the built-in led for visual feedback that the button has been pressed
const int button = D0;       // Connect your button to pin #13

const char* ssid = "Startrek";          // WiFi SSID
const char* password = "Face fade12#";  // WiFi Password

const char* mqtt_server = "192.168.0.123";  // MQTT IP Address
const char* mqtt_topic = "ButtonStatus";    // MQTT Topic Name
const char* clientID = "ESP";               // The client id identifies the ESP8266 device.

Bounce bouncer = Bounce();  // Initialise the Pushbutton Bouncer object

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
  
  bouncer.update();   // Update button state

  if (bouncer.rose()) {
    digitalWrite(ledPin, LOW);

    if (client.publish(mqtt_topic, "Button pressed!")) {      // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
      Serial.println("Button pushed and message sent!");
    }
  
    else {                                                    //client.publish will return a boolean value depending on whether it succeded or not.
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10);                                              // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(mqtt_topic, "Button pressed!");
    }
  }
  else if (bouncer.fell()) {
    // Turn light on when button is released
    digitalWrite(ledPin, HIGH);
  }
} // void loop ends here

void ConnectToWiFi(void){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
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
