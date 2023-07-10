#pragma once
#include "Arduino.h"
#include "Timer.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

class Tachometer
{
private:
    const int analogPin;
    unsigned long timeWhenHighStart = 0;
    unsigned long timeWhenLowStart = 0;
    unsigned long timeHigh = 0;
    unsigned long timeLow = 0;
    bool rollingValue;
    int maxMeasure = 3;

public:
    unsigned long dutyCycle = 0;
    Tachometer(int analogPin)
        : analogPin(analogPin)
    {
    }
    void begin()
    {
        pinMode(analogPin, INPUT);
    }

    bool getDigital()
    {
        if (analogRead(analogPin) > 10)
            return HIGH;
        else
            return LOW;
    }
    bool update()
    {
        int measureCounter = 0;
        do
        {
            bool lastState = getDigital();
            rollingValue = lastState;

            if (rollingValue)
                timeWhenHighStart = micros();
            else
                timeWhenLowStart = micros();

            while (rollingValue == lastState)
            {
                rollingValue = getDigital();
            }

            if (lastState)
                timeHigh = micros() - timeWhenHighStart;
            else
                timeLow = micros() - timeWhenLowStart;
            /* 
            measureCounter++;
            if (measureCounter > maxMeasure)
                break;*/
            dutyCycle = timeHigh + timeLow;
        } while (abs(timeHigh - timeLow) > 0.10 * dutyCycle || dutyCycle < 3000); // change this value as needed
        return false;
    }
    bool update_old()
    {
        bool stateAtBeginning = getDigital();
        bool stateAfterChange;
        unsigned long firstDurationStart = 0;
        unsigned long firstDurationEnd = 0;
        unsigned long secondDurationStart = 0;
        unsigned long secondDurationEnd = 0;

        while (true)
        {
            if (getDigital() != stateAtBeginning)
            {
                firstDurationStart = micros();
                break;
            }
        }

        while (true)
        {

            if (getDigital() == stateAtBeginning)
            {
                firstDurationEnd = micros();
                break;
            }
        }

        while (true)
        {
            if (getDigital() != stateAtBeginning)
            {
                secondDurationStart = micros();
                break;
            }
        }
        while (true)
        {
            if (getDigital() == stateAtBeginning)
            {
                secondDurationStart = micros();
                break;
            }
        }
        dutyCycle = secondDurationStart - firstDurationStart;
        if (dutyCycle > 5300)
            return true;
        else
            return false;
    }

    unsigned long measureAverageDutyCycle(int numMeasurements, double outlierThreshold)
    {
        std::vector<unsigned long> measurements;
        for (int i = 0; i < numMeasurements; i++)
        {
            update();
            measurements.push_back(dutyCycle);
        }

        // Calculate mean and standard deviation
        double mean = std::accumulate(measurements.begin(), measurements.end(), 0.0) / numMeasurements;
        double sq_sum = std::inner_product(measurements.begin(), measurements.end(), measurements.begin(), 0.0);
        double stdDev = std::sqrt(sq_sum / numMeasurements - mean * mean);

        // Remove outliers
        measurements.erase(std::remove_if(measurements.begin(), measurements.end(),
                                          [mean, stdDev, outlierThreshold](unsigned long x)
                                          {
                                              return std::abs(x - mean) > outlierThreshold * stdDev;
                                          }),
                           measurements.end());

        // Recalculate mean
        mean = std::accumulate(measurements.begin(), measurements.end(), 0.0) / measurements.size();

        return static_cast<unsigned long>(mean);
    }
};
