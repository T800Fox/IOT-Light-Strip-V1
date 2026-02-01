#include "AfterDark.hpp"

AfterDark::AfterDark()
{
    storedSunrise = -1;
    storedSunset = -1;
    storedTime = -1;
}

AfterDark::~AfterDark()
{
}

void AfterDark::initialise()
{
    while (storedSunrise == -1 && storedSunset == -1)
    {
        fetchSunriseSunset(&storedSunrise, &storedSunset);

        if(storedSunrise == -1 && storedSunset == -1)
        {
            Serial.println("Failed to get Sunrise/Sunset, retrying...");
            delay(5000);
        }
    }
    ssLastUpdated = millis();

    while (storedTime == -1 )
    {
        fetchDatetime(&storedTime);

        if(storedTime == -1 )
        {
            Serial.println("Failed to get Local Datetime, retrying...");
            delay(5000);
        }
    }
    dtLastUpdated = millis();
}

// WiFiClientSecure AfterDark::httpsGetRequest(const char* site, const char* request)
// {
//     // Connect to HTTP server
//     WiFiClientSecure client;
//     client.setInsecure();
//     client.setTimeout(10000);
//     Serial.print("[HTTPS] Attempting connection to ");
//     Serial.print(site);
//     Serial.print("...");
//     if (!client.connect(site, 443))
//     {
//         Serial.println("refused");
//         return client;
//     }
//     Serial.println("Connected!");

//     // client.print(String("GET ") + request + " HTTP/1.1\r\n" +
//     //              "Host: " + host + "\r\n" +
//     //              "User-Agent: BuildFailureDetectorESP8266\r\n" +
//     //              "Connection: close\r\n\r\n");

//     // Send HTTP request
//     client.print("GET ");
//     client.print(request);
//     client.println(" HTTP/1.0");
//     client.print("Host: ");
//     client.println(site);
//     client.println(F("Connection: close"));
//     if (client.println() == 0)
//     {
//         Serial.println(F("Failed to send request"));
//         client.stop();
//         return client;
//     }

//     // Check HTTP status
//     char status[32] = {0};
//     client.readBytesUntil('\r', status, sizeof(status));
//     // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
//     if (strcmp(status + 9, "200 OK") != 0)
//     {
//         Serial.print("[HTTPS] Unexpected response code: ");
//         Serial.println(status);
        
//         client.stop();
//         return client;
//     } 
//     else
//     {
//         Serial.println("[HTTPS] Response Code: 200");
//     }

//     // Skip HTTP headers
//     char endOfHeaders[] = "\r\n\r\n";
//     if (!client.find(endOfHeaders))
//     {
//         Serial.println(F("Invalid response"));
//         client.stop();
//         return client;
//     }

//     return client;
// }

int AfterDark::DatetimeToInt(const char* datetimeString, int pos)
{
    String inputBuffer = datetimeString;
    String outputBuffer = "";

    Serial.print("[DatetimeConv] Extracing Datetime Data - ");
    Serial.println(datetimeString);
    Serial.print(outputBuffer);

    if(pos == 0)
    {
        Serial.print("[DatetimeConv] Extracting Hour : ");
        outputBuffer = inputBuffer[0];
        Serial.print(outputBuffer);

        if(inputBuffer.indexOf(":") != 1)
        {
            outputBuffer = outputBuffer + inputBuffer[1];
            Serial.print(" -> ");
            Serial.print(outputBuffer);
        }

    }
    else if(pos == 1)
    {
        Serial.print("[DatetimeConv] Extracting Minute : ");
        outputBuffer = inputBuffer[inputBuffer.indexOf(":") + 1];
        Serial.print(outputBuffer);

        if (inputBuffer[inputBuffer.indexOf(":") + 3] == ':')
        {
            outputBuffer = outputBuffer + inputBuffer[inputBuffer.indexOf(":") + 2];
            Serial.print(" -> ");
            Serial.print(outputBuffer);
        }
    }
    Serial.println(" ");
    return outputBuffer.toInt();
}

bool AfterDark::isNight()
{
    bool night = false;
    
    Serial.print("Time since SS update : ");
    Serial.println(millis() - ssLastUpdated);
    if (ssUpdateRate < (millis() - ssLastUpdated))
    {
        int successSS = fetchSunriseSunset(&storedSunrise, &storedSunset);
        if(successSS == 1)
        {
            Serial.println("Successful SS Update");
            ssLastUpdated = millis();
        }
    }

    Serial.print("Time since DT update : ");
    Serial.println(millis() - dtLastUpdated);
    if (dtUpdateRate < (millis() - dtLastUpdated))
    {
        int successDt = fetchDatetime(&storedTime);
        if(successDt == 1)
        {
            Serial.println("Successful DT Update");
            dtLastUpdated = millis();
        }
    }


    if (storedTime < storedSunrise || storedTime > storedSunset)
    {
        night = true;
    }

    return night;
}

int AfterDark::fetchSunriseSunset(int* riseMins, int* setMins)
{
    Serial.print("[HTTP] Attempting connection to local api...");
    WiFiClient client;
    JsonDocument doc; 

    if (!client.connect("192.168.1.100", 5000))
    {
        Serial.println("failed");
        return 0;
    }
    Serial.println("Connected!");

    client.println(F("GET /sydSunriseSunset HTTP/1.0"));
    client.println(F("Host: 192.168.1.100"));
    client.println(F("Connection: close"));
    // client.println("Cache-Control: no-cache"); 192.168.1.100/sydDatetime
    // client.println("User-Agent: arduino/1.0");
    client.println();

    if (client.println() == 0)
    {
        Serial.println("[HTTP] Failed to send request");
        client.stop();
        return 0;
    }

    Serial.print("[HTTP] Waiting");
    while(client.connected() && !client.available()) 
    {
        delay(1);
        Serial.print(".");
    }
    Serial.println("ready");

    // while(client.connected() || client.available())
    // {
    //     char c = client.read();
    //     Serial.print(c);
    //     // Serial.println(status);
    // }

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0)
    {
        Serial.print("[HTTP] Unexpected response - Code: ");
        Serial.println(status);

        while(client.available())
        {
            char c = client.read();
            Serial.print(c);
            // Serial.println(status);
        }
        
        client.stop();
        return 0;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
        Serial.println("[HTTP] Invalid response");
        client.stop();
        return 0;
    }

    // Allocate the JSON document
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    DeserializationError error = deserializeJson(doc, client);
    if (error)
    {
        Serial.print("[HTTP] deserializeJson() failed: ");
        Serial.println(error.f_str());
            while (client.available())
            {
                char c = client.read();
                Serial.print(c);
            }
        client.stop();

        return 0;
    }

    // Disconnect
    client.stop();
    Serial.println("[HTTP] Success");

    int sunriseHour = DatetimeToInt((const char*)doc["results"]["sunrise"],0);
    int sunriseMin = DatetimeToInt((const char *)doc["results"]["sunrise"], 1);
    int sunsetHour = DatetimeToInt((const char *)doc["results"]["sunset"], 0);
    int sunsetMin = DatetimeToInt((const char *)doc["results"]["sunset"], 1);

    *riseMins = sunriseHour*60 + sunriseMin;
    *setMins = sunsetHour*60 + sunsetMin;

    return 1;
}

int AfterDark::fetchDatetime(int* timeMins)
{
    Serial.print("[HTTP] Attempting connection to local api...");
    WiFiClient client;
    JsonDocument doc;

    if (!client.connect("192.168.1.100", 5000))
    {
        Serial.println("failed");
        return 0;
    }
    Serial.println("Connected!");

    client.println(F("GET /sydDatetime HTTP/1.0"));
    client.println(F("Host: 192.168.1.100"));
    client.println(F("Connection: close"));
    // client.println("Cache-Control: no-cache"); 192.168.1.100/sydDatetime
    // client.println("User-Agent: arduino/1.0");
    client.println();

    if (client.println() == 0)
    {
        Serial.println("[HTTP] Failed to send request");
        client.stop();
        return 0;
    }

    Serial.print("[HTTP] Waiting");
    while(client.connected() && !client.available()) 
    {
        delay(1);
        Serial.print(".");
    }
    Serial.println("ready");

    // while(client.connected() || client.available())
    // {
    //     char c = client.read();
    //     Serial.print(c);
    //     // Serial.println(status);
    // }

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0)
    {
        Serial.print("[HTTP] Unexpected response - Code: ");
        Serial.println(status);

        while(client.available())
        {
            char c = client.read();
            Serial.print(c);
            // Serial.println(status);
        }
        
        client.stop();
        return 0;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
        Serial.println("[HTTP] Invalid response");
        client.stop();
        return 0;
    }

    // Allocate the JSON document
    // Use https://arduinojson.org/v6/assistant to compute the capacity.
    DeserializationError error = deserializeJson(doc, client);
    if (error)
    {
        Serial.print("[HTTP] deserializeJson() failed: ");
        Serial.println(error.f_str());
            while (client.available())
            {
                char c = client.read();
                Serial.print(c);
            }
        client.stop();

        return 0;
    }

    // Disconnect
    client.stop();
    Serial.println("[HTTP] Success");

    // Parse Data
    int hours = (int)doc["hour"];
    int minute = (int)doc["minute"];
    *timeMins = hours*60 + minute;
    
    return 1;
}