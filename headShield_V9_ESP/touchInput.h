#pragma once

class touchInput
{
public:
    int pin;
    int threshold;
    long int pressTime = 80;
    long int releaseTime = 200;
    long int timeWhenPressed = 0;
    bool timeStartedFlag = false;
    touchInput(int pin, int threshold)
    {
        this->pin = pin;
        this->threshold = threshold;
    }
    int readRaw()
    {
        return touchRead(pin);
    }
    bool readAtTheMoment()
    {
        if (touchRead(pin) < threshold)
            return true;
        else
            return false;
    }

    bool read()
    {
        if (readAtTheMoment())
        {
            if (!timeStartedFlag)
            {
                timeWhenPressed = millis();
                timeStartedFlag = true;
            }
            if (millis() - timeWhenPressed > pressTime && millis() - timeWhenPressed < releaseTime)
            {
                timeStartedFlag = false;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            timeStartedFlag = false;
            return false;
        }
    }
};