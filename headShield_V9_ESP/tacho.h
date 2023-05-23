#pragma once
#include "Arduino.h"
#include "Timer.h"

class Tachometer
{
private:
    const int analogPin;
    Timer maxTimer;
    int lastAnalogValue = 0;
    int newAnalogValue = 0;
    int pulseCount = 0;
    unsigned long currentTime = 0;
    unsigned long pulseStart = 0;
    unsigned long totalPulseDuration = 0;

    int minStateChanges;
    int maxStateChanges;

public:
    float rpm = 0.0f; // Add this line to declare the rpm variable.
    float averagePulseWidth = 0.0f;

    Tachometer(int analogPin, int minStateChanges, int maxStateChanges, unsigned long maxTime)
        : analogPin(analogPin),
          minStateChanges(minStateChanges),
          maxStateChanges(maxStateChanges),
          maxTimer(maxTime)
    {
    }
    void begin()
    {
        pinMode(analogPin, INPUT);
    }
    void update()
    {
        maxTimer.preTime = millis();
        while (!maxTimer.timeElapsedMillis())
        {
            newAnalogValue = analogRead(analogPin);
            currentTime = millis();

            // since the LOW value is consistently zero
            if (fallingEdge(lastAnalogValue, newAnalogValue)) // falling edge detected
            {
                pulseCount++;
                totalPulseDuration += currentTime - pulseStart;
            }
            else if (risingEdge(lastAnalogValue, newAnalogValue)) // rising edge detected
            {
                pulseStart = currentTime;
            }

            lastAnalogValue = newAnalogValue;
        }

        if (pulseCount > 0)
        {
            averagePulseWidth = (float)totalPulseDuration / pulseCount;
                   
        }
        

        resetVariables();
    }

private:
    bool risingEdge(int oldValue, int newValue)
    {
        return newValue != 0 && oldValue == 0;
    }

    bool fallingEdge(int oldValue, int newValue)
    {
        return newValue == 0 && oldValue != 0;
    }

    void resetVariables()
    {
        lastAnalogValue = 0;
        pulseCount = 0;
        pulseStart = 0;
        totalPulseDuration = 0;
    }
};
