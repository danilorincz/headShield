#pragma once
struct Timer
{
    unsigned long int preTime;
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
    bool timeElapsed()
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
};
