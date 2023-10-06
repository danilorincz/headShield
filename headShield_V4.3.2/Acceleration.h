#include <vector>
#include <Arduino.h>
#include <algorithm>
#include "timer.h"
class Acceleration
{
private:
    std::vector<int> values;          // array to hold the tacho values
    std::vector<unsigned long> times; // array to hold the timestamps
    int count = 0;                    // count of measurements taken

public:
    Acceleration()
    {
        values.reserve(1000); // Reserve space for up to 1000 values
        times.reserve(1000);  // Reserve space for up to 1000 timestamps
    }

    // Add a new tacho value to the buffer
    void addValue(int value)
    {
        unsigned long currentTime = millis();
        values.push_back(value);
        times.push_back(currentTime);
        count++;
    }

    // Calculate the acceleration based on the values obtained in the last X milliseconds
    float calculateAcceleration(int X)
    {
        if (count == 0)
            return 0;

        unsigned long currentTime = millis();
        unsigned long startTime = currentTime - X; // X seconds ago

        int firstValue = 0;
        int lastValue = 0;
        int receivedValues = 0;

        // Remove old values and their timestamps
        while (!times.empty() && times.front() < startTime)
        {
            times.erase(times.begin());
            values.erase(values.begin());
            count--;
        }

        if (count < (X / 1000) * 10)
        {
            static Timer logErrorTimer(1000);
            if (logErrorTimer.timeElapsedMillis())
                Serial.println("E1");
            return -10;
        }

        firstValue = values.front();
        lastValue = values.back();

        int deltaV = lastValue - firstValue;
        float acceleration = (float)deltaV / count;

        return acceleration;
    }
};

class MinMax
{
private:
    std::vector<float> values;        // array to hold the tacho values
    std::vector<unsigned long> times; // array to hold the timestamps
    int count = 0;                    // count of measurements taken

public:
    MinMax()
    {
        values.reserve(1000); // Reserve space for up to 1000 values
        times.reserve(1000);  // Reserve space for up to 1000 timestamps
    }

    // Add a new tacho value to the buffer
    void addValue(float value)
    {
        unsigned long currentTime = millis();
        values.push_back(value);
        times.push_back(currentTime);
        count++;
    }

    // Get the maximum value in the last X seconds
    float getMaxValue(float duration)
    {
        return getExtremeValue(duration, true);
    }

    // Get the minimum value in the last X seconds
    float getMinValue(int duration)
    {
        return getExtremeValue(duration, false);
    }

private:
    float getExtremeValue(int duration, bool findMax)
    {
        unsigned long currentTime = millis();
        unsigned long startTime = currentTime - duration; // duration seconds ago

        // Remove old values and their timestamps
        while (!times.empty() && times.front() < startTime)
        {
            times.erase(times.begin());
            values.erase(values.begin());
            count--;
        }

        if (count == 0)
            return 0;

        if (findMax)
        {
            return *std::max_element(values.begin(), values.end());
        }
        else
        {
            return *std::min_element(values.begin(), values.end());
        }
    }
};