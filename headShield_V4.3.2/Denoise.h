#pragma once

#include "Arduino.h"
#include "Timer.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <deque>
// this is just an example of the movingAverage class.

class MovingAverage
{
private:
    std::vector<int> values; // Vector to hold the measurements
    int index = 0;           // Current index into the array
    int sum = 0;             // Sum of the measurements
    int count = 0;           // Count of measurements taken
    int size;                // Size of the moving average

public:
    // Constructor to initialize the size and resize the vector
    MovingAverage(int size)
        : size(size)
    {
        values.resize(size);
    }

    void add(int value)
    {
        if (count < size)
            count++;                // Increase count until we have 'size' elements
        sum -= values[index];       // Subtract the old value from the sum
        sum += value;               // Add the new value to the sum
        values[index] = value;      // Store the new value in the array
        index = (index + 1) % size; // Move to the next position in the array
    }

    float average()
    {
        if (count < size) // If there are less than 'size' measurements
            return -1;    // Return -1
        else
            return static_cast<float>(sum) / count; // Calculate and return the average
    }
};

class Denoise
{
private:
    int numMeasurements;
    double outlierTreshold;

    int smoothSize;
    std::deque<unsigned long> smoothValues;

    int averageSize;
    std::deque<unsigned long> averageValues;

    MovingAverage movingAverage;

public:
    Denoise(int numMeasurements, double outlierThreshold, int smoothSize, int averageSize)
        : numMeasurements(numMeasurements),
          outlierTreshold(outlierThreshold),
          smoothSize(smoothSize),
          averageSize(averageSize),
          movingAverage(smoothSize) // Initialize the MovingAverage object with the smoothSize
    {
    }

    // No changes to this function
    unsigned long measureAverageDutyCycle(int (*getRawValue)())
    {
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

        return static_cast<unsigned long>(mean);
    }

    unsigned long getMovingAverage()
    {
        unsigned long newMeasurement = measureAverageDutyCycle(); // Get new measurement
        movingAverage.add(newMeasurement);                        // Add the new measurement to the MovingAverage object
        smoothValues.push_back(movingAverage.average());          // Add the new average to the smooth values

        // If we have more than smoothSize values
        if (smoothValues.size() > smoothSize)
        {
            smoothValues.pop_front(); // Remove the oldest value
        }

        // Calculate the average of the smooth values
        unsigned long meanSmoothValue = std::accumulate(smoothValues.begin(), smoothValues.end(), 0) / smoothValues.size();
        return meanSmoothValue;
    }

    void updateFinalValue()
    {
        unsigned long movingAverage = getMovingAverage();
        averageValues.push_back(movingAverage);

        // If we have more than averageSize values
        if (averageValues.size() > averageSize)
        {
            averageValues.pop_front(); // Remove the oldest value
        }
    }

    unsigned int getFinalValue()
    {
        updateFinalValue();

        // If we have enough averaged values
        if (averageValues.size() >= averageSize)
        {
            unsigned long finalMean = std::accumulate(averageValues.begin(), averageValues.end(), 0) / averageValues.size();
            return static_cast<unsigned int>(finalMean);
        }
        else
        {
            return 0; // If not enough values gathered, return 0 or some other indicator of incomplete data
        }
    }
};

