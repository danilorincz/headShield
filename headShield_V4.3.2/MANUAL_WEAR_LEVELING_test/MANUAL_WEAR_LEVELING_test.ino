#include <Preferences.h>

Preferences data;
#define FAN_PIN 2
unsigned long fanOnTime = 0;
unsigned long fanTurnedOnTime = 0;
const unsigned long writeInterval = 10;
const unsigned long maxWritesPerLocation = 1000;
unsigned long writeCount = 0;
int locationIndex = 0;
unsigned long lastWriteTime = 0;
bool fanWasOn = false;
bool fanIsOn = false;
String serialInput;
void setup()
{
    Serial.begin(115200);
    pinMode(FAN_PIN, INPUT);
    data.begin("my-app", false);
    locationIndex = data.getUInt("locationIndex", 0);
    writeCount = data.getUInt("writeCount", 0);
    String locationKey = String("fanOnTime") + String(locationIndex);
    fanOnTime = data.getULong(locationKey.c_str(), 0);
}

void printLatestFanOnTime()
{
    int latestLocationIndex = data.getUInt("locationIndex", 0);
    unsigned long latestWriteCount = data.getUInt("writeCount", 0);
    String locationKey = String("fanOnTime") + String(latestLocationIndex);
    unsigned long latestFanOnTime = data.getULong(locationKey.c_str(), 0);
    Serial.print("Latest fanOnTime from flash memory: ");
    Serial.println(latestFanOnTime);
}

void writeFanOnTimeToFlash()
{
    writeCount++;
    if (writeCount >= maxWritesPerLocation)
    {
        locationIndex++;
        writeCount = 0;
    }
    String locationKey = String("fanOnTime") + String(locationIndex);
    data.putULong(locationKey.c_str(), fanOnTime);
    data.putUInt("locationIndex", locationIndex);
    data.putUInt("writeCount", writeCount);
    lastWriteTime = millis();
}
unsigned long logTime = 0;
void loop()
{
    if (fanIsOn)
    {
        if (fanTurnedOnTime == 0)
        {
            fanTurnedOnTime = millis();
        }
        fanOnTime += millis() - fanTurnedOnTime;
        fanTurnedOnTime = millis();

        if (millis() - lastWriteTime > writeInterval * 1000UL)
        {
            writeFanOnTimeToFlash();
        }
    }
    else if (fanWasOn)
    {
        fanOnTime += millis() - fanTurnedOnTime;
        writeFanOnTimeToFlash();
        fanTurnedOnTime = 0;
    }
    else
    {
        fanTurnedOnTime = 0;
    }

    fanWasOn = fanIsOn;

    if (millis() - logTime > 1000)
    {
        printLatestFanOnTime();
        logTime = millis();
    }
}
void serailInputRead()
{

    while (Serial.available() > 0)
    {
        char incomingByte = Serial.read();
        if (incomingByte == '\n')
        {
            if (serialInput == "1")
            {
                fanIsOn = true;
            }
            else if (serialInput == "0")
            {
                fanIsOn = false;
            }
            serialInput = "";
        }
        else
        {
            serialInput += incomingByte;
        }
    }
}