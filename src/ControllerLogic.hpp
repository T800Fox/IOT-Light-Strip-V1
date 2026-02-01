#ifndef CONTROLLER_LOGIC_H
#define CONTROLLER_LOGIC_H

#include <Arduino.h>
#include "Strip.hpp"
#include "AfterDark.hpp"

class ControllerLogic
{
    public:
        enum state
        {
            OFF = 0,
            MOTION = 1,
            MANUAL = 2
        };

        using colour = rgb::ledVal;
        
        ControllerLogic(/* args */);
        ~ControllerLogic();

        void initialise();

        void runLoop(unsigned long lastMotion, int aLightLevel);

        int getState();
        void setState(int aState);

        unsigned long getKeepOnTime();
        void setKeepOnTime(unsigned long aMillis);

        void setLightThreshold(int aThreshold);

        void setStripColour(colour aColour);

    private:
        bool colourMatch(colour aColourOne, colour aColourTwo);

        rgb::Strip strip;
        AfterDark timeOfDayApi;

        int currentState;
        unsigned long keepOnTime;
        int storedLightThreshold;

        colour storedColour;
};

#endif