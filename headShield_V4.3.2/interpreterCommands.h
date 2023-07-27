//* INTERPRETER
#include "basic.h"

StatData tachoAnalysis(int numberOfMeasurements, int measurementsMultiplier)
{
    fan.on();
    int counter = 0;
    int allTimeMax = 0;
    int allTimeMin = 9999;
    StatData dataNow;
    Statistics statisticsNow(numberOfMeasurements);

    while (true)
    {
        tacho.getAverage();

        if (statisticsNow.addValue(tacho.finalValue))
        {
            counter++;
            if (counter > measurementsMultiplier)
                break;

            if (counter > 7) // skip the first seven
            {
                dataNow = statisticsNow.getStats();
                Serial.print("new set of value added: ");
                Serial.println(dataNow.average);
                if (dataNow.min < allTimeMin)
                    allTimeMin = dataNow.min;
                if (dataNow.max > allTimeMax)
                    allTimeMax = dataNow.max;
            }
        }
    }
    dataNow.min = allTimeMin;
    dataNow.max = allTimeMax;
    fan.off();
    return dataNow;
}
void refreshLimits(int condition) // {0 -> no air flow} {1 -> normal}  {2 -> no filter} {3 -> fan fault}
{
    StatData newLimits = tachoAnalysis(400, 20);
    bool writeToFlash = false;
    StatData oldLimitsClone;

    switch (condition)
    {
    case 0:
        oldLimitsClone = noAir.getStatData();
        break;
    case 1:
        oldLimitsClone = normal.getStatData();
        break;
    case 2:
        oldLimitsClone = noFilter.getStatData();
        break;
    case 3:
        oldLimitsClone = faultFan1.getStatData();
        break;
    }

    if (newLimits.max > oldLimitsClone.max || newLimits.min < oldLimitsClone.min)
    {
        writeToFlash = true;
    }

    if (writeToFlash)
    {
        switch (condition)
        {
        case 0:
            putData(newLimits, data, "noAirLimits");
            break;
        case 1:
            normal.setLimit(newLimits);
            putData(newLimits, data, "normalLimits");
            break;
        case 2:
            noFilter.setLimit(newLimits);
            putData(newLimits, data, "noFilterLimits");
            break;
        case 3:
            faultFan1.setLimit(newLimits);
            putData(newLimits, data, "faultFan1Limits");
            break;
        }
    }

    if (writeToFlash)
        Serial.println("Values changed! (min and max)");
    else
        Serial.println("Old limits NOT changed to new limits!");

    Serial.println("Prev limits: ");
    Serial.print("   ");
    Serial.println(oldLimitsClone.max);
    Serial.print("   ");
    Serial.println(oldLimitsClone.min);

    Serial.print("New limits: ");
    Serial.print("   ");
    Serial.println(newLimits.max);
    Serial.print("   ");
    Serial.println(newLimits.min);
}

namespace interpretCommand
{
    namespace print
    {
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
            Serial.println("noAir");
            Serial.print("  MAX: ");
            Serial.println(noAir.getMax());
            Serial.print("  MIN: ");
            Serial.println(noAir.getMin());
            Serial.print("  AVE: ");
            Serial.println(noAir.getAverage());

            Serial.println("Normal");
            Serial.print("  MAX: ");
            Serial.println(normal.getMax());
            Serial.print("  MIN: ");
            Serial.println(normal.getMin());
            Serial.print("  AVE: ");
            Serial.println(normal.getAverage());

            Serial.println("noFilter");
            Serial.print("  MAX: ");
            Serial.println(noFilter.getMax());
            Serial.print("  MIN: ");
            Serial.println(noFilter.getMin());
            Serial.print("  AVE: ");
            Serial.println(noFilter.getAverage());

            Serial.println("faultFan1");
            Serial.print("  MAX: ");
            Serial.println(faultFan1.getMax());
            Serial.print("  MIN: ");
            Serial.println(faultFan1.getMin());
            Serial.print("  AVE: ");
            Serial.println(faultFan1.getAverage());
        }
        void bareLimitValue()
        {

            Serial.println("Bare limit values");

            Serial.println(normal.getAverage());
            Serial.println(normal.getMax());
            Serial.println(normal.getMin());

            Serial.println(noFilter.getMax());
            Serial.println(noFilter.getMin());

            Serial.println(faultFan1.getMax());
            Serial.println(faultFan1.getMin());

            Serial.println(battery.percent);
            Serial.println(visor.state);
            Serial.println(" ");
        }
    }
    void refreshNoAirLimit()
    {
        refreshLimits(0);
    }
    void refreshNormalLimit()
    {
        refreshLimits(1);
    }
    void refreshNoFilterLimits()
    {
        refreshLimits(2);
    }
    void refreshFaultFan1Limits()
    {
        refreshLimits(3);
    }
    void clearAllData()
    {
        nvs_flash_erase();
        nvs_flash_init();
        noAir.clear();
        normal.clear();
        noFilter.clear();
        faultFan1.clear();
        Serial.println("All data cleared!");
    }
    void toggleSerialEnable()
    {
        serialEnabled = !serialEnabled;
    }
    void recalculateFromNormal()
    {
        int base;
        int A = 41;
        int B = 46;
        int C = 60;
        int D = 66;
        StatData newLimits = tachoAnalysis(1000, 20);
        newLimits.max += 5;
        newLimits.min -= 5;
        normal.setLimit(newLimits);
        putData(newLimits, data, "normalLimits");
        base = newLimits.average;

        noAir.setMin(normal.getMax() + 1);
        noAir.setMax(normal.getMax() + 50);
        noFilter.setMax(base - A);
        noFilter.setMin(base - B);
        faultFan1.setMax(base - C);
        faultFan1.setMin(base - D);

        putData(noAir.getStatData(), data, "noAirLimits");
        putData(normal.getStatData(), data, "normalLimits");
        putData(noFilter.getStatData(), data, "noFilterLimits");
        putData(faultFan1.getStatData(), data, "faultFan1Limits");

        Serial.println("_______________");
        print::limitValue();
        Serial.println(" ");
        Serial.println(" ");
        Serial.println(" ");
    }

}

using namespace interpretCommand::print;
Interpreter printTachoValue("tacho", tachoValue);
Interpreter printBattery("battery", batteryValue);
Interpreter printPeriferial("periferial", periferialValue);
Interpreter printLimits("print limit", limitValue);
Interpreter printBareLimits("limit", bareLimitValue);

using namespace interpretCommand;
Interpreter analyseNoAir("noAir", refreshNoAirLimit);
Interpreter analyseNormal("normal", refreshNormalLimit);
Interpreter analysenoFilter("noFilter", refreshNoFilterLimits);
Interpreter analysefaultFan1("faultFan1", refreshFaultFan1Limits);
Interpreter clearLimits("clear", clearAllData);
Interpreter toggleSerial("enable", toggleSerialEnable);
Interpreter doRecalculation("recalc", recalculateFromNormal);