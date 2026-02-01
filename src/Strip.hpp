#ifndef STRIP_V0_H
#define STRIP_V0_H

#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 13
#define NUM_LEDS 230

namespace rgb
{
    struct ledVal
    {
        int r;
        int g;
        int b;
    };
    
    class Strip
    {
        public:
            Strip(/* args */);
            ~Strip();

            bool validColour(ledVal aColour);
            bool isOn();
            void setColour(ledVal aColour);
            ledVal getColour();

        private:
            ledVal maxVal;
            ledVal currentVal;
            CRGB leds[NUM_LEDS];
    };
} // namespace rgb


#endif