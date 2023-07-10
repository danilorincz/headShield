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
        } while (abs(timeHigh - timeLow) > 0.10 * dutyCycle || dutyCycle < 3450); // change this value as needed
        return false;
    }

    unsigned long measureAverageDutyCycle_old(int numMeasurements, double outlierThreshold, bool (*getOut)())
    {
        std::vector<unsigned long> measurements;
        for (int i = 0; i < numMeasurements; i++)
        {
            if (getOut())
                return 0;
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

    unsigned long measureAverageDutyCycle(int numMeasurements, double outlierThreshold, bool (*getOut)())
    {
        auto start = millis();

        unsigned long sum = 0;
        unsigned long sumSq = 0;

        std::deque<unsigned long> measurements;
        for (int i = 0; i < numMeasurements; i++)
        {
            if (getOut())
                return 0;
            update();
            measurements.push_back(dutyCycle);
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

        auto end = millis();
        Serial.print("Execution time: ");
        Serial.print(end - start);
        Serial.println(" ms");

        return static_cast<unsigned long>(mean);
    }
};
