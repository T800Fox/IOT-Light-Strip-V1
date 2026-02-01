#ifndef AFTER_DARK_H
#define AFTER_DARK_H

#include <Arduino.h>

#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

class AfterDark
{

    public:
        AfterDark();
        ~AfterDark();

        void initialise();
        bool isNight();

    private:
        WiFiClientSecure httpsGetRequest(const char* site, const char* request);

        int fetchSunriseSunset(int* riseMins, int* setMins);
        int fetchDatetime(int* timeMins);

        int DatetimeToInt(const char* datetimeString, int pos);

        WiFiClientSecure clientStream;
        

        int storedSunrise;  
        int storedSunset;
        unsigned long ssUpdateRate = 3600000; // hour
        unsigned long ssLastUpdated;

        int storedTime;
        unsigned long dtUpdateRate = 120000; // 2 min 
        unsigned long dtLastUpdated;
};

#endif 