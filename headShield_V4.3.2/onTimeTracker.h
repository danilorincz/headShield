#pragma once

#include <Preferences.h>

class OnTimeTracker
{
private:
    Preferences data;
    unsigned long deviceOnTime;
    unsigned long deviceTurnedOnTime;
    const unsigned long writeInterval;
    const unsigned long maxWritesPerLocation;
    unsigned long writeCount;
    int locationIndex;
    unsigned long lastWriteTime;
    bool deviceWasOn;

public:
    OnTimeTracker(const unsigned long writeInterval = 60, const unsigned long maxWritesPerLocation = 1000) : writeInterval(writeInterval), maxWritesPerLocation(maxWritesPerLocation),
                                                                                                             deviceOnTime(0), deviceTurnedOnTime(0), writeCount(0), locationIndex(0), lastWriteTime(0), deviceWasOn(false)
    {
    }

    void begin()
    {
        data.begin("app", false);
        locationIndex = data.getUInt("locationIndex", 0);
        writeCount = data.getUInt("writeCount", 0);
        String locationKey = String("deviceOnTime") + String(locationIndex);
        deviceOnTime = data.getULong(locationKey.c_str(), 0);
    }

    void update(bool deviceIsOn)
    {
        if (deviceIsOn)
        {
            if (deviceTurnedOnTime == 0)
            {
                deviceTurnedOnTime = millis();
            }
            deviceOnTime += millis() - deviceTurnedOnTime;
            deviceTurnedOnTime = millis();

            if (millis() - lastWriteTime > writeInterval * 1000UL)
            {
                writeCount++;
                if (writeCount >= maxWritesPerLocation)
                {
                    locationIndex++;
                    writeCount = 0;
                }
                String locationKey = String("deviceOnTime") + String(locationIndex);
                data.putULong(locationKey.c_str(), deviceOnTime);
                data.putUInt("locationIndex", locationIndex);
                data.putUInt("writeCount", writeCount);
                lastWriteTime = millis();
            }
        }
        else if (deviceWasOn)
        {
            deviceOnTime += millis() - deviceTurnedOnTime;
            writeCount++;
            if (writeCount >= maxWritesPerLocation)
            {
                locationIndex++;
                writeCount = 0;
            }
            String locationKey = String("deviceOnTime") + String(locationIndex);
            data.putULong(locationKey.c_str(), deviceOnTime);
            data.putUInt("locationIndex", locationIndex);
            data.putUInt("writeCount", writeCount);
            lastWriteTime = millis();
            deviceTurnedOnTime = 0;
        }
        else
        {
            deviceTurnedOnTime = 0;
        }
        deviceWasOn = deviceIsOn;
    }

    unsigned long getLatestOnTime()
    {
        int latestLocationIndex = data.getUInt("locationIndex", 0);
        String locationKey = String("deviceOnTime") + String(latestLocationIndex);
        return data.getULong(locationKey.c_str(), 0);
    }

    void printLatestOnTime()
    {
        unsigned long latestOnTime = getLatestOnTime();
        Serial.print("Latest device on-time from flash memory: ");
        Serial.println(latestOnTime);
    }
};