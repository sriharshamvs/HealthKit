# RPi
import time
import paho.mqtt.client as mqtt
import os

#mqtt_topic = "HK"
mqtt_topic_bpm  = "HK/BPM"
#mqtt_topic_ecg  = "HK/ECG"
#mqtt_topics = ["HK/BPM", "HK/ECG"]

csv_count = 0

mqtt_broker_ip = "192.168.43.98"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(mqtt_topics)
    client.subscribe(mqtt_topic_bpm)

# The callback for when a PUBLISH message is received from the server.

def on_message(client, userdata, msg):
    msg_payload = str(msg.payload.decode('utf-8'))
    print("Topic: ",msg.topic +"\nMessage: "+msg_payload)
    if msg_payload != "Button pressed!":
        if msg_payload.startswith("B"):
            writeToFile_BPM(msg_payload)
        if msg_payload.startswith("E"):
            writeToFile_ECG(msg_payload)

# This function will write the incoming BPM data to a file
def writeToFile_BPM(Message_Payload):
    print("Writing To File")
    with open('/home/mvs/Workspace/MajorProject/HKlogs/bpm.csv', 'a') as f:
        #f.write(time.strftime('%D') + "," + time.strftime('%H:%M:%S') + ","+ Message_Payload)
        f.write(Message_Payload)
        f.write('\n')
    return

# This function will write the incoming BPM data to a file
def writeToFile_ECG(Message_Payload):
    print("Writing To File")
    with open('/home/mvs/Workspace/MajorProject/HKlogs/ecg.csv', 'a') as f:
        #f.write(time.strftime('%D') + "," + time.strftime('%H:%M:%S') + ","+ Message_Payload)
        f.write(Message_Payload)
        f.write('\n')
    return

# Create MQTT client and connect to localhost, i.e. the Raspberry Pi running
# this script and the MQTT server.
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost",1883, 60)
# Connect to the MQTT server and process messages in a background thread.
client.loop_start()
# Main loop to listen for button presses.
print('Script is running, press Ctrl-C to quit...')
while True:
    # Look for a change from high to low value on the button input to
    # signal a button press.
    time.sleep(0.02)  # Delay for about 20 milliseconds to debounce.
    #client.publish('/leds/esp8266', 'TOGGLE')
