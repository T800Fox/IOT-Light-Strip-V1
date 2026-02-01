#ifndef REQUESTER_H
#define REQUESTER_H

#include <Arduino.h>

#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

int sunriseSunsetDatetime(const char* datetimeString, int pos);
int getSunriseSunset(int* riseMins, int* setMins);
int getDatetime();

#endif 