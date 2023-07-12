#pragma once

class Touch
{
private:
    int pin;
    int thresholdMin;
    int thresholdMax;

private: //* SINGLE TAP
    int singleTapTimeMax = 400;
    int singleTapTimeMin = 10;
    bool released;
    long int singleTouchStartTime;

public:
    bool singleTouchInProgress = false;

private: //* LONG TAP
    bool longTouchInProgress = false;
    unsigned long longTouchStartTime = 0;
    unsigned long longTapTreshold = 1000;

public:
    Touch(int pin, int thresholdMin, int thresholdMax) : pin(pin), thresholdMin(thresholdMin), thresholdMax(thresholdMax) {}

    int getAnalog()
    {
        touchRead(pin);
        return touchRead(pin);
    }

    bool getDigital()
    {
        int value = getAnalog();
        if (value > thresholdMin && value < thresholdMax)
        {
            return true;
        }
        else
            return false;
    }
    bool singleTap()
    {
        if (getDigital())
        {
            if (!singleTouchInProgress)
            {
                singleTouchStartTime = millis();
                singleTouchInProgress = true;
            }
        }
        else if (singleTouchInProgress)
        {
            bool returnValue = false;
            unsigned long touchEndTime = millis();
            unsigned long touchDuration = touchEndTime - singleTouchStartTime;
            if (touchDuration > singleTapTimeMin && touchDuration < singleTapTimeMax)
            {
                singleTouchInProgress = false;
                returnValue = true;
            }
            singleTouchInProgress = false;
            singleTouchStartTime = 0;
            return returnValue;
        }
        return false;
    }

    bool longTap()
    {
        if (!getDigital())
        {
            longTouchInProgress = false;
            longTouchStartTime = 0;
            return false;
        }
        else
        {
            if (!longTouchInProgress)
            {
                longTouchStartTime = millis();
                longTouchInProgress = true;
            }
            if ((millis() - longTouchStartTime) > longTapTreshold && longTouchInProgress)
            {
                longTouchInProgress = false;
                return true;
            }
        }
        return false;
    }
};
