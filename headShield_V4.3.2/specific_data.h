bool recoverDeviceSpecificData(String loadedID)
{
    bool OK = false;

    if (loadedID == "HS_1")
    {
        Serial.println("LOADING FOR HS_1");
        battery.a = 1.23;
        battery.b = -1.35;

        ADAPT.NOFILTER_CONST_MAX = 3535;
        ADAPT.NORMAL_CONST_MIN = 3555;
        ADAPT.NORMAL_CONST_MAX = 3590;
        ADAPT.NOAIR_CONST_MIN = 3605;

        OK = true;
    }
    if (loadedID == "HS_2")
    {
        Serial.println("LOADING FOR HS_2");
        battery.a = 0.92;
        battery.b = 0.56;

        ADAPT.NOFILTER_CONST_MAX = 3610;
        ADAPT.NORMAL_CONST_MIN = 3635;
        ADAPT.NORMAL_CONST_MAX = 3655;
        ADAPT.NOAIR_CONST_MIN = 3670;

        OK = true;
    }
    if (loadedID == "HS_3")
    {
        Serial.println("LOADING FOR HS_3");
        battery.a = 1.02;
        battery.b = -0.11;

        ADAPT.NOFILTER_CONST_MAX = 3590;
        ADAPT.NORMAL_CONST_MIN = 3615;
        ADAPT.NORMAL_CONST_MAX = 3635;
        ADAPT.NOAIR_CONST_MIN = 3645;

        OK = true;
    }
    if (loadedID == "HS_4")
    {
        Serial.println("LOADING FOR HS_4");
        battery.a = 0.94;
        battery.b = 0.48;

        ADAPT.NOFILTER_CONST_MAX = 3575;
        ADAPT.NORMAL_CONST_MIN = 3585;
        ADAPT.NORMAL_CONST_MAX = 3625;
        ADAPT.NOAIR_CONST_MIN = 3645;

        OK = true;
    }

    if (OK)
    {
        ADAPT.ABSOLUTE_MIN = 3400;
        ADAPT.ABSOLUTE_MAX = 3800;

        ADAPT.NORMAL_INITIAL_MIN = ADAPT.NORMAL_CONST_MIN - 30;
        ADAPT.NORMAL_INITIAL_MAX = ADAPT.NORMAL_CONST_MAX + 10;

        ADAPT.LOWER_DIF = 25;
        ADAPT.UPPER_DIF = 15;
        ADAPT.MAX_ACCEL = 0.65;
        normal.setMax(ADAPT.NORMAL_INITIAL_MAX);
        normal.setMin(ADAPT.NORMAL_INITIAL_MIN);
        return true;
    }
    else
    {
        Serial.println("Problem with the SSID specific data!");
        return false;
    }
}
