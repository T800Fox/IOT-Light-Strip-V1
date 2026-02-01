#include "requester.h"

String rawPayload;
String cleanPayload;

WiFiClientSecure clientStream;

JsonDocument doc;

// to be used on sunrisesunset json
// pos : 0 -> hour , 1 -> minute
int sunriseSunsetDatetime(const char* datetimeString, int pos)
{
    String inputBuffer = datetimeString;
    String outputBuffer = "";

    Serial.print("[u8g2] Extracing Datetime Data - ");
    Serial.println(datetimeString);
    Serial.print(outputBuffer);

    if(pos == 0)
    {
        Serial.print("[u8g2] Extracting Hour : ");
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
        Serial.print("[u8g2] Extracting Minute : ");
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

WiFiClientSecure altHttpsGETRequest(const char* site, const char* request)
{
    // Connect to HTTP server
    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(10000);

    Serial.print("[ALT HTTPS] Attempting connection to ");
    Serial.print(site);
    Serial.print("...");
    if (!client.connect(site, 443))
    {
        Serial.println("refused");
        return client;
    }

    Serial.println("Connected!");

    // client.print(String("GET ") + request + " HTTP/1.1\r\n" +
    //              "Host: " + host + "\r\n" +
    //              "User-Agent: BuildFailureDetectorESP8266\r\n" +
    //              "Connection: close\r\n\r\n");

    // Send HTTP request
    client.print("GET ");
    client.print(request);
    client.println(" HTTP/1.0");
    client.print("Host: ");
    client.println(site);
    client.println(F("Connection: close"));
    if (client.println() == 0)
    {
        Serial.println(F("Failed to send request"));
        client.stop();
        return client;
    }

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0)
    {
        Serial.print("[ALT HTTPS] Unexpected response code: ");
        Serial.println(status);
        
        client.stop();
        return client;
    } 
    else
    {
        Serial.println("[ALT HTTPS] Response Code: 200");
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
        Serial.println(F("Invalid response"));
        client.stop();
        return client;
    }

    return client;
}

int getSunriseSunset(int* riseMins, int* setMins)
{
    clientStream = altHttpsGETRequest("api.sunrisesunset.io", "/json?lat=-33.89146&lng=151.13825&time_format=24");
    

    // Allocate the JSON document
    doc = NULL;

    // can get data even if client disconnected, for some reason???
    if (clientStream.connected() == false)
    {
        Serial.println("[JSON] Client already disconnected.");
        // return response;
    }

    // Parse JSON object
    DeserializationError error = deserializeJson(doc, clientStream);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());

        clientStream.stop();

        return 0;
    }

    // Extract values
    // Serial.print(F("[JSON] Sunrise: "));
    // Serial.println(doc["results"]["sunrise"].as<const char *>());

    // Disconnect
    clientStream.stop();
    Serial.println("[HTTP] Success");

    int sunriseHour = sunriseSunsetDatetime((const char*)doc["results"]["sunrise"],0);
    int sunriseMin = sunriseSunsetDatetime((const char *)doc["results"]["sunrise"], 1);
    int sunsetHour = sunriseSunsetDatetime((const char *)doc["results"]["sunset"], 0);
    int sunsetMin = sunriseSunsetDatetime((const char *)doc["results"]["sunset"], 1);

    *riseMins = sunriseHour*60 + sunriseMin;
    *setMins = sunsetHour*60 + sunsetMin;

    return 1;
}


int getDatetime()
{
    // formerly altGetDatetime()

    Serial.print("[HTTP] Attempting connection to local api...");
    WiFiClient client;

    if (!client.connect("192.168.1.100", 5000))
    {
        Serial.println("failed");
        return -1;
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
        return -1;
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
        return -1;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders))
    {
        Serial.println("[HTTP] Invalid response");
        client.stop();
        return -1;
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

        return -1;
    }

    // Disconnect
    client.stop();
    Serial.println("[HTTP] Success");

    int hours = (int)doc["hour"];
    int minute = (int)doc["minute"];
    int minutesIntoDay = hours*60 + minute;
    
    return minutesIntoDay;

}

