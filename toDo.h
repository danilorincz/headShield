Problems in the code
    //* create a method to turn off the fans completely
    //* long touch input registers tap first
    //* headSensor optimized
    // the sensor must be connected by pushing a button
    // optimize the fan speed reading
    // do a touch calibration before every start
    bool kaki;


bool isStableInput(bool inputState, unsigned long stableTime)
{
    static bool outputState = false;
    static unsigned long stableStartTime = 0;

    if (inputState == true)
    {
        if (stableStartTime == 0)
        {
            stableStartTime = millis();
        }
        else if (millis() - stableStartTime >= stableTime)
        {
            outputState = true;
        }
    }
    else
    {
        outputState = false;
        stableStartTime = 0;
    }

    return outputState;
}

