#include "mqtt.h"

//*****VARIABLER*****
const char* espId = "heating"; //Identity


//WiFi uses const char pointers instead of strings as string is default not part of C. 
//Pointer points to array of char, and effectively acts as (c type)string (note to self)
const char* ssid = "ssid";
const char* password = "password";


const char* mqtt_server = "178.164.41.198"; //IP or DDNS for RPi
const int mqtt_port = 1883;

float setTemperature = 20; //defining var a possible value
float realTemperature = 20; //defining var a possible value

//Class instances of libs.
WiFiClient heating; //Creating an instancce of the WiFiClient called heating
PubSubClient client(heating); //Creating an instance of PubSubClient called heatingClient, using the WiFiClient-instance as parameter input in the constructor

//Setup function for wifi. used to start up and connect.
void wifiSetup() {
  delay(10);
  
  //Debuing
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //Begin function of the WiFi class
  WiFi.begin(ssid, password);

  //Checking connection, checking the returned value with enum class for different connections
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Debuing connection info
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Setting mode "station", the ESP32 connects to an access point, and not acting as access point
  WiFi.mode(WIFI_STA);
}

void callback(char* topic, byte* payload, unsigned int length){
  //Debug stuff
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  //Storing the payload in a string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  //debuging the message
  Serial.println(message);

  //Handle the incoming topics
  if(String(topic) == "setTempOut"){
    setTemperature = message.toFloat();
  }
  if(String(topic) == "tempOut"){
    realTemperature = message.toFloat();
  }
}

//Setup for the mqtt
void mqttSetup(){
  client.setCallback(callback); //setting the callback function as callback function for the client class
  client.setServer(mqtt_server, mqtt_port);
}

void reconnect() {
  // Runs until connected
  Serial.print("Attempting MQTT connection...");
  while (!client.connected()) { //program will not proceed if it can't connect to the MQTT server
    
    // Attempt to connect
    if (client.connect(espId, ssid, password)) {  // Connection to mqtt-server
      Serial.println("connected");
      
      //Subscribe to topics
      client.subscribe("setTempOut");
      client.subscribe("tempOut");

    } else {
      Serial.print("failed, rc=");  //Try again if connection fails.
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000); // waits 5 seconds before trying again


    }
  }
}

//Just for testing purposes
void testMqtt(){
  unsigned long previousMillis = 0;
  const unsigned long interval = 2000;  // 2 seconds
  int i = 0;

  const char* array[] = {"Hei!", "jeg", "sender", "meldinger", "trådløst"};

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    client.publish("test", array[i]);
    i = (i + 1) % sizeof(array);
    previousMillis = currentMillis;
  }
}
