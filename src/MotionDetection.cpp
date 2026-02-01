#include "MotionDetection.hpp"

MotionDetection::MotionDetection()
{
    
}

MotionDetection::~MotionDetection()
{
    sensors.~LinkedList();
}

void MotionDetection::addPIR(int aPin)
{
    int currentId = sensors.getLength();
    sensors.Append({currentId, aPin});
    Serial.print("Adding PIR w/ id [");
    Serial.print(currentId);
    Serial.print("] to pin [");
    Serial.print(aPin);
    Serial.println("]");
}

void MotionDetection::initialise()
{
    timeOfLastMotion = millis();
    if (sensors.moveToStart())
    {
        do
        {
            pinMode(sensors.getCurrent().pin, INPUT);
        } 
        while (sensors.next());
    }
}

void MotionDetection::scan(bool debug)
{
    bool motion = false;
    int currentReading = 0;

    if (debug)
    {
        Serial.print("PIR Readings : [");
    }

    if (sensors.moveToStart())
    {
        do
        {
            currentReading = digitalRead(sensors.getCurrent().pin);
            if (currentReading == HIGH)
            {
                motion = true;
            }

            if (debug)
            {
                Serial.print(currentReading);
                

                if (sensors.getCurrent().id == sensors.Last().id)
                {
                    Serial.println("]");
                }
                else
                {
                    Serial.print("] [");
                }
            }

        } while (sensors.next());
    }

    if (motion)
    {
        timeOfLastMotion = millis();
    }
}

unsigned long MotionDetection::millisSinceMotion()
{
    return millis() - timeOfLastMotion;
}

int MotionDetection::getVal(int aPin)
{
    int value = -1;

    if (sensors.moveToStart())
    {
        do
        {
            if (sensors.getCurrent().pin == aPin)
            {
                value = digitalRead(sensors.getCurrent().pin);
            }

        } while (sensors.next());
    }
    
    return value;
}