#pragma once

class touchInput
{
public:
    int pin;
    int threshold;
    long int pressTime = 100;
    long int releaseTime = 200;
    long int timeWhenPressed = 0;
    bool timeStartedFlag = false;
    touchInput(int pin, int threshold)
    {
        this->pin = pin;
        this->threshold = threshold;
    }
    bool readActual()
    {
        if (touchRead(pin) < threshold)
            return true;
        else
            return false;
    }

    bool read()
    {
        if (readActual())
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
        }
        return false;
    }
};