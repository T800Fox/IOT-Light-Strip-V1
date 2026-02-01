#include "Strip.hpp"

rgb::Strip::Strip(/* args */)
{
    maxVal = {255,255,255};

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

}

rgb::Strip::~Strip()
{
}

bool rgb::Strip::validColour(ledVal aColour)
{
    bool response = true;

    if ( aColour.r < 0 || aColour.g < 0 || aColour.b < 0)
    {
        response = false;
    }

    if (aColour.r > maxVal.r || aColour.g > maxVal.g || aColour.b > maxVal.b)
    {
        response = false;
    }

    return response;
}

bool rgb::Strip::isOn()
{
    if ( currentVal.r > 0 || currentVal.g > 0 || currentVal.b > 0)
    {
        return true;
    }
    
    return false;
}

void rgb::Strip::setColour(ledVal aColour)
{
    if(!validColour(aColour))
    {
        return;
    }

    currentVal = aColour;

    if (isOn())
    {
        digitalWrite(LED_BUILTIN, LOW); // HIGH and LOW are opposite for some reason
    }
    else
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    Serial.println("Setting Strip");
    for (int i = 0; i < NUM_LEDS; i++)
    {
        Serial.print(aColour.r);
        Serial.print(" ");
        Serial.print(aColour.g);
        Serial.print(" ");
        Serial.println(aColour.b);
        leds[i] = CRGB(aColour.r, aColour.g, aColour.b);
        FastLED.show();
    }

    return;
}

rgb::ledVal rgb::Strip::getColour()
{
    return currentVal;
}
