#pragma once

class FunctionRunner
{
    unsigned long interval;
    void (*func)();
    unsigned long lastCall = 0;

public:
    FunctionRunner(void (*func)(), unsigned long interval) : interval(interval), func(func) {}

    void takeAction()
    {
        if (millis() - lastCall > interval)
        {
            lastCall = millis();
            func();
        }
    }
};
