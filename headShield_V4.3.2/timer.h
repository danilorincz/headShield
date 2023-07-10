#pragma once
struct Timer
{
    unsigned long int preTime = 0;
    float duration;

    Timer(float duration)
    {
        this->preTime = 0;
        this->duration = duration;
    }
    void setDuration(int newDuration)
    {
        this->duration = newDuration;
    }
    bool timeElapsedMicros()
    {
        if (micros() - this->preTime > this->duration)
        {
            this->preTime = micros();
            return true;
        }
        else
        {
            return false;
        }
    }
    bool timeElapsedMillis()
    {
        if (millis() - this->preTime > this->duration)
        {
            this->preTime = millis();
            return true;
        }
        else
        {
            return false;
        }
    }
    void reset()
    {
        this->preTime = 0;
    }
};
