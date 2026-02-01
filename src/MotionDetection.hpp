#ifndef MOTION_DETECTION_H
#define MOTION_DETECTION_H

#include <Arduino.h>
#include "LinkedList.hpp"

class MotionDetection
{
    public:
        MotionDetection();
        ~MotionDetection();

        void addPIR(int aPin);
        void initialise();

        void scan(bool debug);
        int getVal(int aPin);
        unsigned long millisSinceMotion();

    private:
        struct pirSensor
        {
            int id;
            int pin;
        };

        LinkedList<pirSensor> sensors;

        unsigned long timeOfLastMotion;
};
#endif 

#include "LinkedList.cpp"