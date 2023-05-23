#pragma once

#include "Arduino.h"
class Tachometer
{
private:
    const int analogPin;
    int analogThreshold;
    const int minStateChanges;
    Timer maxTimer;
    int lastAnalogValue = 0;
    int sampleCount = 0;
    int stateChanges = 0;
    unsigned long pulseStart = 0;
    unsigned long totalPulseWidth = 0;
    int counter = 0;

public:
    int averagePulseWidth = 0;
    Tachometer(int analogPin, int analogThreshold, int minStateChanges, unsigned long maxTime)
        : analogPin(analogPin),
          analogThreshold(analogThreshold),
          minStateChanges(minStateChanges),
          maxTimer(maxTime)
    {
    }
    void begin()
    {
        pinMode(analogPin, INPUT);
    }
    void changeAnalogTreshold(int newValue)
    {
        analogThreshold = newValue;
    }
    void update()
    {
        maxTimer.preTime = millis();
        while (!maxTimer.timeElapsedMillis()) //stateChanges < minStateChanges &&
        {
            int newAnalogValue = analogRead(analogPin);
            unsigned long currentTime = millis();

            if (didStateChange(lastAnalogValue, newAnalogValue))
            {
                stateChanges++;
                sampleCount++;

                if (isRisingEdge(lastAnalogValue, newAnalogValue))
                {
                    pulseStart = currentTime;
                }
                else if (isFallingEdge(lastAnalogValue, newAnalogValue))
                {
                    totalPulseWidth += currentTime - pulseStart;
                }
            }

            lastAnalogValue = newAnalogValue;
        }
        averagePulseWidth = stateChanges;

        resetVariables();
    }

private:
    bool isRisingEdge(int oldValue, int newValue)
    {
        return oldValue < analogThreshold && newValue >= analogThreshold;
    }

    bool isFallingEdge(int oldValue, int newValue)
    {
        return oldValue >= analogThreshold && newValue < analogThreshold;
    }

    bool didStateChange(int oldValue, int newValue)
    {
        return isRisingEdge(oldValue, newValue) || isFallingEdge(oldValue, newValue);
    }

    void resetVariables()
    {
        lastAnalogValue = 0;
        sampleCount = 0;
        stateChanges = 0;
        pulseStart = 0;
        totalPulseWidth = 0;
        maxTimer.preTime = 0;
    }
};
