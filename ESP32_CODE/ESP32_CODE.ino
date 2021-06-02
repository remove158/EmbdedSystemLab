/**
 * A simple Azure IoT example for sending telemetry.
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <String>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

const char* ssid = "OSM_2.4G";
const char* password = "22333555";
const char* mqtt_server = "mqtt.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "fca1de6e-0f39-4ed7-b3c9-3d457a071c2d";
const char* mqtt_username = "FQgaDM3MGyYRwJjhch7u9n2bwCJuhtsH";
const char* mqtt_password = "HPQvdHxb562eV3nxETH(nYrS#A6~H2$g";
int current=1,before=1;
int isInit=0;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int count;
int value = 0;


void callback(char* topic, byte* payload, unsigned int msglength);
int getIntJSON(char* json, char* topic);

void reconnect() {
  while (!client.connected()) {
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      client.setCallback(callback);
      client.subscribe("@private/#");
      client.subscribe("@msg/water");
      client.subscribe("@msg/reset");
      client.publish("@shadow/data/get","");
    }
    else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  client.setServer(mqtt_server, mqtt_port);
  
}



void loop() {
  if (!client.connected()) {
    //reconnect and initialize value
    reconnect();
  }
  client.loop();
  current = digitalRead(23);
  int Checker = Serial.available() ;
  if(Checker && Serial.readString() != "p") {
      String data = "{\"data\": {\"count\":" + String(++count) +  "}}";
      char msg[80];
      data.toCharArray(msg, (data.length() + 1));
      client.publish("@shadow/data/update", msg);
  }
  delay(100); 
}

void callback(char* topic, byte* payload, unsigned int msglength) {
  String message;
  for (int i = 0; i < msglength; i++) {
    message = message + (char)payload[i];
  }
  if(String(topic) == "@private/shadow/data/get/response") {
    // extract bin and totalItems value from message 
    count = getIntJSON(message, "\"count\"");
    isInit = 1;
  }else if (String(topic)== "@msg/water") {

    Serial.println("p");
    String data = "{\"data\": {\"count\":" + String(++count) +  "}}";
    char msg[80];
    data.toCharArray(msg, (data.length() + 1));
    client.publish("@shadow/data/update", msg);
  
  }else if (String(topic) == "@msg/reset") {
    String data = "{\"data\": {\"bias\":" + String(count) +  "}}";
    char msg[80];
    data.toCharArray(msg, (data.length() + 1));
    client.publish("@shadow/data/update", msg);
  }
}

int getIntJSON(String json, String topic) {
  int startTopic = json.indexOf(topic);
  int startInt = json.indexOf(":", startTopic) + 1;
  int stopInt = json.indexOf("}", startInt);
  String valStr = json.substring(startInt, stopInt);
  return valStr.toInt();
}