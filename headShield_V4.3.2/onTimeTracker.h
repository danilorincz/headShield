
#pragma once

#include <Preferences.h>

class OnTimeTracker
{
private:
    Preferences data;
    unsigned long accumulatedOnTime; // Total time the device has been on
    unsigned long lastUpdateTime;    // Last time the 'update' function was called
    int memIdx;                      // Index to keep track of where to write next in flash memory
    bool isUpdated;                  // Flag to check if accumulatedOnTime has been updated
    unsigned long timeSinceLastUpdate;
    unsigned long lastSavedOnTime;
    unsigned long lastSaveTimestamp;
    unsigned long savePeriodTime;
    bool sessionFirstSave = false;
    const int maxKeys = 20;

public:
    OnTimeTracker(unsigned long savePeriodTime) : accumulatedOnTime(0),
                                                  lastUpdateTime(0),
                                                  memIdx(0),
                                                  timeSinceLastUpdate(0),
                                                  lastSavedOnTime(0),
                                                  lastSaveTimestamp(0),
                                                  savePeriodTime(savePeriodTime)
    {
    }

    void begin()
    {
        data.begin("app", false);
        memIdx = data.getUInt("mIdx", 0);
        String locKey = String("aOnT") + String(memIdx);
        accumulatedOnTime = data.getULong(locKey.c_str(), 0);
        data.end();
    }

    unsigned long get_timeOn()
    {
        return accumulatedOnTime;
    }

    unsigned long get_latestSavedOnTime()
    {
        String locKey = String("aOnT") + String(memIdx);
        return data.getULong(locKey.c_str(), 0);
    }

    void update(bool deviceIsOn)
    {
        if (deviceIsOn)
        {
            accumulatedOnTime += (millis() - lastUpdateTime);
            isUpdated = true;
            lastUpdateTime = millis();
        }
    }

    bool save(bool overwrite = false)
    {

        unsigned long currentTime = millis();

        if (((currentTime - lastSaveTimestamp >= savePeriodTime) || overwrite || !sessionFirstSave) && isUpdated)
        {
            data.begin("app", false);
            Serial.println("FILTER TRACKER SAVING");
            if (memIdx >= maxKeys)
            {
                memIdx = 0;
            }
            memIdx++;
            String locKey = String("aOnT") + String(memIdx);
            unsigned long latestOnTime = get_latestSavedOnTime();
            data.putULong(locKey.c_str(), accumulatedOnTime + latestOnTime);
            data.putUInt("mIdx", memIdx);
            isUpdated = false;
            lastSavedOnTime = accumulatedOnTime;
            lastSaveTimestamp = currentTime;
            Serial.print("New time has been saved to flash: ");
            Serial.println(accumulatedOnTime + latestOnTime);
            sessionFirstSave = true;
            data.end();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool clearLatestData()
    {
        data.begin("app", false);
        // Remove all keys from NVS
        for (int i = 1; i <= memIdx; ++i)
        {
            String locKey = String("aOnT") + String(i);
            data.remove(locKey.c_str());
        }

        // Reset memIdx in NVS and in-memory
        data.putUInt("mIdx", 0);
        memIdx = 0;

        // Reset accumulatedOnTime and lastUpdateTime
        accumulatedOnTime = 0;
        lastUpdateTime = 0;

        // Reset sessionFirstSave flag
        sessionFirstSave = false;

        // Debug message
        Serial.println("All data cleared!");
        data.end();
        return true;
    }
};
