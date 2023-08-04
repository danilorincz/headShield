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
    int threshold = 0;

public:
    int finalValue = -1;
    unsigned long dutyCycle = 0;
    Tachometer(int analogPin, int smoothSize)
        : analogPin(analogPin),
          smooth(smoothSize)
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
    int getAnalog()
    {
        return analogRead(analogPin);
    }
    void updateThreshold()
    {
        int value;
        int min = 4095;
        int max = 0;

        int threshold;
        for (int i = 0; i < 20; i++)
        {
            value = getAnalog();
            if (value > max)
                max = value;
            else if (value < min)
                min = value;
        }
        Serial.print("Max: ");
        Serial.println(max);
        Serial.print("Min: ");
        Serial.println(min);

        threshold = (max + min) / 2;

        Serial.print("Threshold: ");
        Serial.println(threshold);
    }
    bool getDigital()
    {
        static bool newValue = false;
        if (getAnalog() > threshold)
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
        //int counter_doWhile = 0;

        do
        {/*
            counter_doWhile++;
            if (counter_doWhile > 20)
                break;*/
            bool lastState = getDigital();
            rollingValue = lastState;

            if (rollingValue)
                timeWhenHighStart = micros();
            else
                timeWhenLowStart = micros();
            //int counter_while = 0;
            while (getDigital() == lastState)
            {/*
                counter_while++;

                if (counter_while > 300)
                {
                    Serial.println("BREAK");
                    break;
                }*/
            }

            if (lastState)
                timeHigh = micros() - timeWhenHighStart;
            else
                timeLow = micros() - timeWhenLowStart;

            dutyCycle = timeHigh + timeLow;

        } while (abs(timeHigh - timeLow) > 0.15 * dutyCycle || dutyCycle < 3500); // change this value as needed
        return true;
    }

    int getAverage()
    {
        if (doMeasure())
            smooth.add(dutyCycle);
        finalValue = smooth.average();
        return finalValue;
    }
};