//* INTERPRETER
#include "basic.h"

StatData tachoAnalysis(int numberOfMeasurements, int measurementsMultiplier)
{
    int counter = 0;
    int allTimeMax = 0;
    int allTimeMin = 9999;
    StatData dataNow;
    Statistics statisticsNow(numberOfMeasurements);

    const unsigned long spinUpTime = 5000;

    while (true)
    {
        int newValue = tacho.getAverage();

        if (fan.getOnTime() > spinUpTime)
        {
            if (statisticsNow.addValue(newValue))
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
    }
    dataNow.min = allTimeMin;
    dataNow.max = allTimeMax;
    return dataNow;
}
void refreshNormal()
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
        Serial.println("Values changed! (min and max)");
    }
    else
        Serial.println("Old limits NOT changed to new limits!");
}

namespace interpretCommand
{
    namespace print
    {
        void sensorValues()
        {
            perkData.log();
        }
        void inputValues()
        {
            Serial.print("Left touch raw: ");
            Serial.println(touchLeft.getAnalog());
            Serial.print("Right touch raw: ");
            Serial.println(touchRight.getAnalog());
            Serial.print("Visor: ");
            Serial.println(visor.state);
            Serial.print("Head sensor: ");
            Serial.println(headSensor.state);
            Serial.print("Percent: ");
            Serial.println(battery.percent);
            Serial.print("Voltage: ");
            Serial.print(battery.voltage);
            Serial.println(" Volt");
        }
        void tachoValue()
        {
            Serial.println(tacho.finalValue);
        }
        void limitValues()
        {
            Serial.println("Normal");
            Serial.print("  MAX: ");
            Serial.println(normal.getMax());
            Serial.print("  MIN: ");
            Serial.println(normal.getMin());
            Serial.print("  AVE: ");
            Serial.println(normal.getAverage());
        }
        void onTime()
        {
            Serial.println(fan.getOnTime());
        }

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
Interpreter printPeriferial("input", inputValues);
Interpreter printLimits("limit", limitValues);

using namespace interpretCommand;
Interpreter analyse_normal("normal", refreshNormal);
Interpreter clearLimits("clear", clearAllData);
Interpreter toggleSerial("enable serial", toggleSerialEnable);
Interpreter printSensorValues("sensor", sensorValues);
Interpreter printFanOnTime("on time", onTime);
