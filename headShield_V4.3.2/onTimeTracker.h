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
        memIdx = data.getUInt("mIdx", 0);                // memoryLocationIndex
        String locKey = String("aOnT") + String(memIdx); // accumulatedOnTime
        accumulatedOnTime = data.getULong(locKey.c_str(), 0);
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
    void setSaveInterval(unsigned long newInterval)
    {
        savePeriodTime = newInterval;
    }
    void update(bool deviceIsOn)
    {
        if (deviceIsOn)
        {
            accumulatedOnTime += (millis() - lastUpdateTime);
            isUpdated = true;
        }
        lastUpdateTime = millis();
    }

    bool save()
    {
        unsigned long currentTime = millis();
        if (isUpdated && currentTime - lastSaveTimestamp >= savePeriodTime)
        {
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
            return true;
        }
        Serial.println("stopped by 3.");
        return false;
    }

    bool clearLatestData()
    {
        if (memIdx > 0)
        {
            String locKey = String("aOnT") + String(memIdx);
            data.remove(locKey.c_str());
            memIdx--;
            data.putUInt("mIdx", memIdx);
            return true;
        }
        else
            return false;
    }
    float get_memoryWear()
    {
        const unsigned long maxWriteEraseCycles = 100000; // Typical for ESP32
        unsigned long totalWrites = memIdx;               // Assuming each increment of memIdx corresponds to a write

        float wearPercentage = ((float)totalWrites / maxWriteEraseCycles) * 100;
        return wearPercentage;
    }
};
