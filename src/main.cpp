#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "wifi_creds.h"

#include "MotionDetection.hpp"
#include "ControllerLogic.hpp"

#define LED_PIN 13
#define NUM_LEDS 230

#define LDR_PIN A0
#define PIR_1_PIN 16
#define PIR_2_PIN 5
#define PIR_3_PIN 12
#define OFF_SW_PIN 2

const char *ssid = SSID;
const char *password = PASS;
const char *mqttServer = "192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

MotionDetection detector;

ControllerLogic controller;

int ldrVal;

void initWifi();
void callback(char *topic, byte *message, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(115200);

  // WIFI + MQTT SETUP
  initWifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
  delay(200);

  // MOTION DETECTION SETUP
  detector.addPIR(PIR_1_PIN);
  detector.addPIR(PIR_2_PIN);
  detector.addPIR(PIR_3_PIN);
  detector.initialise();

  // MISC SETUP
  controller.initialise();
  controller.setState(controller.MOTION);
  controller.setStripColour({5,5,5});
}

void loop() 
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  detector.scan(true);

  unsigned long sinceMotion = detector.millisSinceMotion();
  Serial.print("[main] Millis since motion - ");
  Serial.println(sinceMotion);

  ldrVal = analogRead(LDR_PIN);
  Serial.print("[main] LDR Value - ");
  Serial.println(ldrVal);

  client.publish("hallwayLights/pir/1", String(detector.getVal(PIR_1_PIN)).c_str());
  client.publish("hallwayLights/pir/2", String(detector.getVal(PIR_2_PIN)).c_str());
  client.publish("hallwayLights/pir/3", String(detector.getVal(PIR_3_PIN)).c_str());
  client.publish("hallwayLights/lastMotion", String(sinceMotion).c_str());
  client.publish("hallwayLights/lightReading", String(ldrVal).c_str());


  controller.runLoop(sinceMotion,ldrVal);

  delay(100);
}

// brought straight from v0
void initWifi()
{
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();


  // // yes I could make a general function for the rgb vals, but I dont feel like it ;)
  // if (String(topic) == "hallwayLights/colour/r")
  // {
  //   Serial.println("Recieved New r Value");
  //   int newR = messageTemp.toInt();

  //   if (newR < 0 || newR > 255)
  //   {
  //     Serial.print(newR);
  //     Serial.println(" invalid size!");
  //   }
  //   else
  //   {
  //     Serial.print("Updating r to ");
  //     Serial.println(newR);
  //     rVal= newR;

  //     if (stripOn)
  //     {
  //       setStrip(rVal, bVal, gVal);
  //     }
  //   }
  // }

  // if (String(topic) == "hallwayLights/colour/g")
  // {
  //   Serial.println("Recieved New g Value");
  //   int newG = messageTemp.toInt();

  //   if (newG < 0 || newG > 255)
  //   {
  //     Serial.print(newG);
  //     Serial.println(" invalid size!");
  //   }
  //   else
  //   {
  //     Serial.print("Updating g to ");
  //     Serial.println(newG);
  //     gVal = newG;

  //     if (stripOn)
  //     {
  //       setStrip(rVal, bVal, gVal);
  //     }
  //   }
  // }

  // if (String(topic) == "hallwayLights/colour/b")
  // {
  //   Serial.println("Recieved New b Value");
  //   int newB = messageTemp.toInt();

  //   if (newB < 0 || newB > 255)
  //   {
  //     Serial.print(newB);
  //     Serial.println(" invalid size!");
  //   }
  //   else
  //   {
  //     Serial.print("Updating b to ");
  //     Serial.println(newB);
  //     bVal = newB;

  //     if (stripOn)
  //     {
  //       setStrip(rVal, bVal, gVal);
  //     }
  //   }
  // }

  if (String(topic) == "hallwayLights/keepOnTime")
  {
    Serial.println("Recieved New Keep On Time..");
    int newTime = messageTemp.toInt();

    if (newTime < 0 || newTime> 100000)
    {
      Serial.print(newTime);
      Serial.println("invalid size!");
    }
    else
    {
      Serial.print("Updating keep on time to ");
      Serial.println(newTime);
      controller.setKeepOnTime(newTime);
    }
  }

  if (String(topic) == "hallwayLights/lightThreshold")
  {
    Serial.println("Recieved New Light Threshold...");
    int newThreshold = messageTemp.toInt();

    if (newThreshold < 0 || newThreshold > 1023)
    {
      Serial.print(newThreshold);
      Serial.println("invalid size!");
    }
    else
    {
      Serial.println("Updating threshold to ");
      Serial.print(newThreshold);
      controller.setLightThreshold(newThreshold);
    }
  }

  // if (String(topic) == "hallwayLights/mode")
  // {
  //   Serial.println("Recieved New Mode...");
  //   if((messageTemp == "DEBUG" || messageTemp == "AUTO" || messageTemp == "MANUAL" || messageTemp == "OFF") == false)
  //   {
  //     Serial.print("Mode '");
  //     Serial.print(messageTemp);
  //     Serial.println("' is invalid");
  //     return;
  //   }

  //   operationMode = messageTemp;
  //   Serial.print("Operation Mode set to {");
  //   Serial.print(operationMode);
  //   Serial.println("}");
  //   return;
  // }
}
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");

      client.subscribe("hallwayLights/mode");
      client.subscribe("hallwayLights/lightThreshold");
      client.subscribe("hallwayLights/keepOnTime");
      client.subscribe("hallwayLights/colour/r");
      client.subscribe("hallwayLights/colour/g");
      client.subscribe("hallwayLights/colour/b");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}