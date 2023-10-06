#pragma once

#include <Preferences.h>

class OnTimeTracker
{
private:
    Preferences data;
    unsigned long accumulatedOnTime = 0; // Total time the device has been on
    unsigned long lastUpdateTime = 0;    // Last time the 'update' function was called
    bool isUpdated = false;              // Flag to check if accumulatedOnTime has been updated
    unsigned long lastSaveTimestamp = 0; // Last time data was saved
    unsigned long savePeriodTime;        // Time period for saving data
    bool sessionFirstSave = false;       // Flag for the first save in a session

public:
    // Constructor
    OnTimeTracker(unsigned long savePeriodTime) : savePeriodTime(savePeriodTime) {}

    // Initialize Preferences
    void begin()
    {
        data.begin("app", false);
        data.end();
    }

    // Load saved data
    void loadData()
    {
        data.begin("app", false);
        accumulatedOnTime = data.getULong("accOnTime", 0);
        data.end();
    }

    // Get the accumulated on-time
    unsigned long get_timeOn()
    {
        return accumulatedOnTime;
    }

    // Update the accumulated on-time
    void update(bool deviceIsOn)
    {
        if (deviceIsOn)
        {
            accumulatedOnTime += (millis() - lastUpdateTime);
            isUpdated = true;
            lastUpdateTime = millis();
        }
    }

    // Save the accumulated on-time to NVS
    bool save(bool overwrite = false)
    {
        unsigned long currentTime = millis();
        if (((currentTime - lastSaveTimestamp >= savePeriodTime) || overwrite || !sessionFirstSave) && isUpdated)
        {
            Serial.println("FILTER TRACKER SAVING");
            data.begin("app", false);
            data.putULong("accOnTime", accumulatedOnTime);
            isUpdated = false;
            lastSaveTimestamp = currentTime;
            sessionFirstSave = true;
            data.end();
            return true;
        }
        else
        {
            return false;
        }
    }

    // Clear all saved data
    bool clearAllData()
    {
        data.begin("app", false);
        data.remove("accOnTime");
        accumulatedOnTime = 0;
        lastUpdateTime = 0;
        sessionFirstSave = false;
        data.end();
        return true;
    }
};
