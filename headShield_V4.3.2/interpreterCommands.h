//* INTERPRETER
#include "basic.h"

StatData tachoAnalysis(int numberOfMeasurements, int measurementsMultiplier)
{
    int counter = 0;
    int allTimeMax = 0;
    int allTimeMin = 9999;
    StatData dataNow;
    Statistics statisticsNow(numberOfMeasurements);

    const unsigned long spinUpTime = 3000;
    unsigned long spinStart = millis();
    while (true)
    {
        if (millis() - spinStart > spinUpTime)
        {
            tacho.getAverage();
        }

        if (statisticsNow.addValue(tacho.finalValue))
        {
            counter++;
            if (counter > measurementsMultiplier)
                break;

            dataNow = statisticsNow.getStats();
            Serial.print("new set of value added: ");
            Serial.println(dataNow.average);
            if (dataNow.average < allTimeMin)
                allTimeMin = dataNow.average;
            if (dataNow.average > allTimeMax)
                allTimeMax = dataNow.average;
        }
    }
    dataNow.min = allTimeMin;
    dataNow.max = allTimeMax;
    return dataNow;
}
void refreshLimits() // {0 -> no air flow} {1 -> normal}  {2 -> no filter} {3 -> fan fault}
{
    StatData oldLimitsClone = normal.getStatData();
    StatData newLimits = tachoAnalysis(400, 20);
    bool writeToFlash = false;

    if (newLimits.max > oldLimitsClone.max || newLimits.min < oldLimitsClone.min)
    {
        writeToFlash = true;
    }

    if (writeToFlash)
    {
        putData(newLimits, data, "normal");
        normal.setLimit(newLimits);
    }

    if (writeToFlash)
        Serial.println("Values changed! (min and max)");
    else
        Serial.println("Old limits NOT changed to new limits!");
}

namespace interpretCommand
{
    namespace print
    {
        void printSensorData()
        {
            perkData.log();
        }
        void batteryValue()
        {
            Serial.print("Percent: ");
            Serial.println(battery.percent);
            Serial.print("Voltage: ");
            Serial.print(battery.voltage);
            Serial.println(" Volt");
        }
        void periferialValue()
        {
            Serial.print("Left touch raw: ");
            Serial.println(touchLeft.getAnalog());
            Serial.print("Right touch raw: ");
            Serial.println(touchRight.getAnalog());
            Serial.print("Visor: ");
            Serial.println(visor.state);
            Serial.print("Head sensor: ");
            Serial.println(headSensor.state);
        }
        void tachoValue()
        {
            Serial.println(tacho.finalValue);
        }
        void limitValue()
        {
            Serial.println("Normal");
            Serial.print("  MAX: ");
            Serial.println(normal.getMax());
            Serial.print("  MIN: ");
            Serial.println(normal.getMin());
            Serial.print("  AVE: ");
            Serial.println(normal.getAverage());
        }
        void printNakedLimits()
        {
            Serial.println("Bare limit values (NORMAL");

            Serial.println(normal.getMax());
            Serial.println(normal.getAverage());
            Serial.println(normal.getMin());
        }
    }

    void refresh_normal()
    {
        refreshLimits();
    }

    void clearAllData()
    {
        nvs_flash_erase();
        nvs_flash_init();
        normal.clear();

        Serial.println("All data cleared!");
    }
    void toggleSerialEnable()
    {
        serialEnabled = !serialEnabled;
    }

}

using namespace interpretCommand::print;
Interpreter printTachoValue("tacho", tachoValue);
Interpreter printBattery("battery", batteryValue);
Interpreter printPeriferial("periferial", periferialValue);
Interpreter printLimits("print limit", limitValue);
Interpreter printBareLimits("limit", printNakedLimits);

using namespace interpretCommand;
Interpreter analyse_normal("normal", refresh_normal);

Interpreter clearLimits("clear", clearAllData);
Interpreter toggleSerial("enable", toggleSerialEnable);

Interpreter printSensorValues("print sensor", printSensorData);
