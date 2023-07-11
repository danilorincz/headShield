#pragma once
#include "Arduino.h"
#include "Timer.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <deque>
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
    bool lastMeasure[4]; // lastMeasure[0] -> oldest lastMeasure[1] -> previous
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
    bool fastMajority(bool a, bool b, bool c)
    {
        return (a & b) | (b & c) | (a & c);
    }
    bool getDigital()
    {
        static bool newValue = false;
        if (analogRead(analogPin) > 0)
        {
            newValue = true;
        }
        else
        {
            newValue = false;
        }

        lastMeasure[0] = lastMeasure[1];
        lastMeasure[1] = lastMeasure[2];
        lastMeasure[2] = lastMeasure[3];
        lastMeasure[3] = newValue;

        int sum = lastMeasure[0] + lastMeasure[1] + lastMeasure[2] + lastMeasure[3];

        return fastMajority(lastMeasure[0], lastMeasure[1], lastMeasure[2]);
    }

    bool update_original()
    {
        int loopCounter = 0;
        do
        {
            if (loopCounter > 50)
                return false;
            loopCounter++;
            bool lastState = getDigital();
            rollingValue = lastState;

            if (rollingValue)
                timeWhenHighStart = micros();
            else
                timeWhenLowStart = micros();

            while (getDigital() == lastState)
            {
            }

            if (lastState)
                timeHigh = micros() - timeWhenHighStart;
            else
                timeLow = micros() - timeWhenLowStart;

            dutyCycle = timeHigh + timeLow;

        } while (abs(timeHigh - timeLow) > 0.05 * dutyCycle || dutyCycle < 3500); // change this value as needed
        return true;
    }

    bool update()
    {
        static bool lastState;
        lastState = getDigital();

        if (lastState)
            timeWhenHighStart = micros();
        else
            timeWhenLowStart = micros();

        while (getDigital() == lastState)
        {
        }

        if (lastState)
            timeHigh = micros() - timeWhenHighStart;
        else
            timeLow = micros() - timeWhenLowStart;

        dutyCycle = timeHigh + timeLow;

        if (abs(timeHigh - timeLow) < 0.02 * dutyCycle && dutyCycle > 580)
            return true;
        else
            return true;
    }

    unsigned long measureAverageDutyCycle(int numMeasurements, double outlierThreshold, bool (*getOut)())
    {
        unsigned long sum = 0;
        unsigned long sumSq = 0;

        std::deque<unsigned long> measurements;
        for (int i = 0; i < numMeasurements; i++)
        {
            if (getOut())
                return 0;
            if (update())
                measurements.push_back(dutyCycle);
            else
                Serial.println("error");
            sum += dutyCycle;
            sumSq += dutyCycle * dutyCycle;
        }

        double mean = sum / static_cast<double>(measurements.size());
        double sq_sum = sumSq;
        double stdDev = std::sqrt(sq_sum / measurements.size() - mean * mean);

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

/*
    bool update_new()
    {
        unsigned long firstChangeStart = 0;
        unsigned long secondChangeStart = 0;
        unsigned long thirdChangeStart = 0;

        bool initialState = getDigital();

        while (true)
        {
            if (getDigital() != initialState)
            {
                firstChangeStart = micros();
                break;
            }
        }

        while (true)
        {
            if (getDigital() == initialState)
            {
                secondChangeStart = micros();
                break;
            }
        }

        while (true)
        {
            if (getDigital() != initialState)
            {
                thirdChangeStart = micros();
                break;
            }
        }

        unsigned long firstDuration = secondChangeStart - firstChangeStart;
        unsigned long secondDuration = thirdChangeStart - secondChangeStart;

        dutyCycle = thirdChangeStart - firstChangeStart;
        if (abs(firstDuration - secondDuration) < 0.10 * dutyCycle && dutyCycle > 3600)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

*/
