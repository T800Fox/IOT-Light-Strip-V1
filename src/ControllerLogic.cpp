#include "ControllerLogic.hpp"

ControllerLogic::ControllerLogic(/* args */)
{
    currentState = OFF;
    keepOnTime = 30000;
    storedLightThreshold = 100;
}

ControllerLogic::~ControllerLogic()
{
}

void ControllerLogic::initialise()
{
    Serial.println("[Controller] Initialiser Called");
    
    storedColour = {25,0,0};
    strip.setColour({5,0,5}); // low white 

    timeOfDayApi.initialise();

    strip.setColour({0,0,0});
}


void ControllerLogic::runLoop(unsigned long lastMotion, int aLightLevel)
{
    switch (currentState)
    {
        case MANUAL:
            Serial.print("MANUAL");
            if (!strip.isOn() || !colourMatch(storedColour, strip.getColour()) )
            {   
                Serial.print(" - Updating Colour");
                strip.setColour(storedColour);
            }
            Serial.println(" ");

            break;

        case OFF:
            Serial.print("OFF");
            if (strip.isOn())
            {   
                Serial.print(" - Turning off");
                strip.setColour({0,0,0});
            }
            Serial.println(" ");

            break;

        case MOTION:
            Serial.print("MOTION");

            bool night = timeOfDayApi.isNight();
            Serial.print(" - NIGHT: ");
            Serial.print(night);

            if(lastMotion <= keepOnTime && aLightLevel <= storedLightThreshold && night)
            {
                if (!strip.isOn())
                {
                    Serial.print(" - Turning on");
                    strip.setColour({25,0,0});
                }
            }
            else
            {
                if (strip.isOn())
                {
                    Serial.print(" - Turning off");
                    strip.setColour({0,0,0});
                    Serial.print(" - STATE : ");
                    Serial.print(currentState);
                }
            }
            Serial.println(" ");

            break;
    }
}

int ControllerLogic::getState()
{
    return currentState;
}

void ControllerLogic::setState(int aState)
{
    if (aState == OFF || aState == MOTION || aState == MANUAL)
    {
        Serial.print("          State set to ");
        Serial.println(aState);
        currentState = aState;
    } 
    else
    {
        Serial.println("[Controller Logic] Invalid State Supplied");
    }
}

void ControllerLogic::setStripColour(colour aColour)
{
    if (strip.validColour(aColour))
    {
        storedColour = aColour;
    }
    else
    {
        Serial.println("[Controller Logic] Invalid Colour");
    }
}

bool ControllerLogic::colourMatch(colour aColourOne, colour aColourTwo)
{
    bool result = true;

    if (aColourOne.r != aColourTwo.r)
    {
        result = false;
    }
    else if (aColourOne.g != aColourTwo.g)
    {
        result = false;
    }
    else if (aColourOne.b != aColourTwo.b)
    {
        result = false;
    }

    return result;
}

void ControllerLogic::setLightThreshold(int aThreshold)
{
    if (aThreshold >= 0 && aThreshold < 1023)
    {
        storedLightThreshold = aThreshold;
    }
}

void ControllerLogic::setKeepOnTime(unsigned long aMillis)
{
    keepOnTime = aMillis;
}