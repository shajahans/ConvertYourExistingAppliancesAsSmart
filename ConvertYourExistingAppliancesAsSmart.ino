/*
 Version 0.4 - April 26 2019
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries (use the correct version)
#include <StreamString.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "Add your API Key from Siniric Here" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "Shajahan" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "Add Your WiFi Password" // TODO: Change to your Wifi network password

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 
#define LED 2
int MillisCount = 0;

const int relayPin1 = 16; // D0 Living Room Light
const int relayPin2 = 5; //  D1 Living Room Fan
const int relayPin3 = 4; //  D2 Bed Room Light
const int relayPin4 = 0; //  D3 Bed Room Fan
const int relayPin5 = 3; //  D4 Guest Room Fan (Changing to Rx)
const int relayPin6 = 14; // D5 Guest Room Light
const int relayPin7 = 12; // D6 Kitchen Light
const int relayPin8 = 13; // D7 Kitchen Fan

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;


// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId) {
  if (deviceId == "5ec77412f2f71462da8a4323143") // Device ID of Living Room Light
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin1, LOW);
  } 
  else if (deviceId == "5ec77428f2f71462da8a32423") // Device ID of Living Room Fan
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin2, LOW);
  }
  else if (deviceId == "5ec77440f2f71462da8a4324234") // Device ID of Bed Room Light
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin3, LOW);
  }
  else if (deviceId == "5ec77450f2f71462da8a453455") // Device ID of Bed Room Fan
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin4, LOW);
  }
  else if (deviceId == "5ec77462f2f71462da8a42344") // Device ID of Guest Fan
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin5, LOW);
  }
  else if (deviceId == "5ec77470f2f71462da8a324324") // Device ID of Guest Light
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin6, LOW);
  }
  else if (deviceId == "5ec7a335f2f71462da8a342524358") // Device ID of Kitchen Light
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin7, LOW);
  }
  else if (deviceId == "5ec7a349f2f71462da82345342e") // Device ID of Kitchen Fan
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin8, LOW);
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5ec77412f2f71462da8a24355f") // Device ID of Living Room Light
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin1, HIGH);
   }
   else if (deviceId == "5ec77428f2f71462da8a435455") // Device ID of Living Room Fan
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin2, HIGH);
  }
     else if (deviceId == "5ec77440f2f71462da8a453455c") // Device ID of Bed Room Light
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin3, HIGH);
  }
     else if (deviceId == "5ec77450f2f71462da4352435901") // Device ID of Bed Room Fan
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin4, HIGH);
  }
     else if (deviceId == "5ec77462f2f71462da8543543507") // Device ID of Guest Room Fan
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin5, HIGH);
  }
     else if (deviceId == "5ec77470f2f71462da8a435450b") // Device ID of Guest Room Light
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin6, HIGH);
  }
     else if (deviceId == "5ec7a335f2f71462da8454528") // Device ID of Kitchen Light
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin7, HIGH);
  }
     else if (deviceId == "5ec7a349f2f71462da8a435455e") // Device ID of Kitchen Fan
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(relayPin8, HIGH);
  }
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif        
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  pinMode(relayPin5, OUTPUT);
  pinMode(relayPin6, OUTPUT);
  pinMode(relayPin7, OUTPUT);
  pinMode(relayPin8, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);
  digitalWrite(relayPin5, HIGH);
  digitalWrite(relayPin6, HIGH);
  digitalWrite(relayPin7, HIGH);
  digitalWrite(relayPin8, HIGH);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
    if (MillisCount == 10000)
    digitalWrite(LED, LOW);

  if (MillisCount == 20000)
    {
      digitalWrite(LED, HIGH);
      MillisCount = 0;
    }
 
  MillisCount += 1;
}
