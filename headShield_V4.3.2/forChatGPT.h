
I want to modify this snippet from a technical documentation:
"
The encoder-generated signal is subject to noise due to electronic characteristics, necessitating the use of noise filtering and smoothing algorithms. The system can record thousands of raw data points per second, a number that can fluctuate depending on processor load. Each measurement point is evaluated for validity by the software; outliers are discarded. The system then calculates a smoothed value using the geometric mean, updated based on the previous 600 measurements. After completing 600 measurement cycles, the system pauses briefly before restarting the measurement cycle. This process is repeated 100 times.
During these cycles, the device maintains continuous serial communication (UART) with a computer. Upon cycle completion, the minimum and maximum values of the measured speed range are displayed on the monitor, defining the parameters for the given profile.
Before initiating the program, the physical condition was established, and then the device began automatic measurements. Data capture took approximately 70 seconds. Measurements were conducted on four devices.
"
Here are the C++ code which perfrom the analysis. Modify the text and correct it according to this:


//This will run 400x100 (numberOfMeasurements x measurementsMultiplier) times.
StatData tachoAnalysis(int numberOfMeasurements, int measurementsMultiplier)
{
    int counter = 0;
    int allTimeMax = 0;
    int allTimeMin = 9999;
    StatData dataNow;
    Statistics statisticsNow(numberOfMeasurements);

    const unsigned long spinUpTime = 5000;

    while (true)
    {
        int newValue = tacho.getAverage();

        if (fan.getCurrentSessionOn() > spinUpTime)
        {
            if (statisticsNow.addValue(newValue))
            {
                counter++;
                if (counter > measurementsMultiplier)
                    break;

                dataNow = statisticsNow.getStats();
                Serial.print("new set of value added: ");
                Serial.println(dataNow.average);
                if (dataNow.average < allTimeMin)
                    allTimeMin = dataNow.average;
                if (dataNow.average > allTimeMax)
                    allTimeMax = dataNow.average;
            }
        }
    }
    dataNow.min = allTimeMin;
    dataNow.max = allTimeMax;
    return dataNow;
}


void refreshNormal()
{
    StatData oldLimitsClone = normal.getStatData();
    StatData newLimits = tachoAnalysis(400, 100);
    bool writeToFlash = false;

    if (newLimits.max > oldLimitsClone.max || newLimits.min < oldLimitsClone.min)
    {
        writeToFlash = true;
    }

    if (writeToFlash)
    {
        putData(newLimits, data, "normal");
        normal.setLimit(newLimits);
        Serial.println("Values changed! (min and max)");
    }
    else
        Serial.println("Old limits NOT changed to new limits!");
}


// And this is the struct that was utilized by the analysis function


#include <vector>
#include <algorithm>

struct StatData
{
    unsigned int min;
    unsigned int max;
    unsigned int average;
};

class Statistics
{
private:
    std::vector<int> values;
    unsigned long sum = 0;
    unsigned int requiredSize;
    StatData data;

public:
    Statistics(unsigned int size) : requiredSize(size), values() {}

    bool addValue(int value)
    {
        values.push_back(value);
        sum += value;

        if (values.size() == requiredSize)
        {
            // Calculate the statistics
            auto minmax = std::minmax_element(values.begin(), values.end());
            data.min = *minmax.first;
            data.max = *minmax.second;
            data.average = sum / values.size();

            // Clear the vector and reset the sum
            values.clear();
            sum = 0;
            return true;
        }
        return false;
    }

    StatData getStats()
    {
        return data;
    }
};


// This is the core class for measuring the signals from the encoder:
#pragma once
#include "Arduino.h"
#include "..\Timer.h"
#include "..\movingAverage.h"

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
    bool lastMeasure[3];
    MovingAverage smooth;

public:
    bool warning = true;
    int finalValue = -1;
    unsigned long dutyCycle = 0;
    Tachometer(int analogPin, int smoothSize)
        : analogPin(analogPin),
          smooth(smoothSize)
    {
    }
    void begin()
    {
        pinMode(analogPin, INPUT_PULLDOWN);
    }
    void warningOn()
    {
        warning = true;
    }
    void warningOff()
    {
        warning = false;
    }
    bool fastMajority(bool a, bool b, bool c)
    {
        return (a & b) | (b & c) | (a & c);
    }
    int getAnalog()
    {
        return analogRead(analogPin);
    }

    bool getDigital()
    {
        static bool newValue = false;
        if (getAnalog() > 0)
        {
            newValue = true;
        }
        else
        {
            newValue = false;
        }

        lastMeasure[0] = lastMeasure[1];
        lastMeasure[1] = lastMeasure[2];
        lastMeasure[2] = newValue;

        return fastMajority(lastMeasure[0], lastMeasure[1], lastMeasure[2]);
    }

    bool doMeasure()
    {
        int counter_doWhile = 0;
        int counter_while = 0;
        bool returnValue = false;

        while (true)
        {
            counter_doWhile++;

            bool lastState = getDigital();
            rollingValue = lastState;

            if (rollingValue)
                timeWhenHighStart = micros();
            else
                timeWhenLowStart = micros();
            while (getDigital() == lastState)
            {
                counter_while++;
                if (counter_while >= 200)
                    break;
            }

            if (lastState)
                timeHigh = micros() - timeWhenHighStart;
            else
                timeLow = micros() - timeWhenLowStart;

            if (counter_doWhile >= 8 || counter_while >= 200)
            {
                returnValue = false;
                break;
            }

            dutyCycle = timeHigh + timeLow;
            if (abs(timeHigh - timeLow) < 0.15 * dutyCycle && dutyCycle > 3500) //* GOOD
            {
                returnValue = true;
                break;
            }
        }

        return returnValue;
    }

    int getAverage()
    {
        if (doMeasure())
            smooth.add(dutyCycle);
        else
            Serial.println("Measurement failed");
        finalValue = smooth.average();
        return finalValue;
    }
};