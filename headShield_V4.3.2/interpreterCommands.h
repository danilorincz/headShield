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
            Serial.print("new set of value added_");
            Serial.println(counter);
            counter++;
            dataNow = statisticsNow.getStats();
            if (counter > 5) // skip the first three
            {
                if (dataNow.min < allTimeMin)
                    allTimeMin = dataNow.min;
                if (dataNow.max > allTimeMax)
                    allTimeMax = dataNow.max;
            }
        }
        if (counter > measurementsMultiplier)
        {
            break;
        }
    }
    fan.off();
    return dataNow;
}
void refreshLimits(int condition) // {0 -> no air flow} {1 -> normal}  {2 -> no filter} {3 -> fan fault}
{
    StatData newLimits = tachoAnalysis(300, 15);

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

    StatData movingLimits = oldLimitsClone;
    if (newLimits.max > movingLimits.max)
    {
        writeToFlash = true;
        movingLimits.max = newLimits.max;
    }
    if (newLimits.min < movingLimits.min)
    {
        writeToFlash = true;
        movingLimits.min = newLimits.min;
    }

    if (writeToFlash)
    {
        switch (condition)
        {
        case 0:
            newLimits.max += 100;
            noAir.setLimit(newLimits);
            putData(newLimits, data, "noAirLimits");
            break;
        case 1:
            newLimits.max += 5;
            newLimits.min -= 5;
            normal.setLimit(newLimits);
            putData(newLimits, data, "normalLimits");
            break;
        case 2:
            newLimits.max += 5;
            newLimits.min -= 5;
            noFilter.setLimit(newLimits);
            putData(newLimits, data, "noFilterLimits");
            break;
        case 3:
            newLimits.max += 5;
            newLimits.min -= 5;
            faultFan1.setLimit(newLimits);
            putData(newLimits, data, "faultFan1Limits");
            break;
        }
    }

    if (writeToFlash)
    {
        Serial.println("Values changed! (min and max)");

        Serial.print("Prev limits: ");
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
    else
    {
        Serial.println("Old limits NOT changed to new limits!");

        Serial.print("Old limits: ");
        Serial.print("   ");
        Serial.println(oldLimitsClone.max);
        Serial.print("   ");
        Serial.println(oldLimitsClone.min);

        Serial.print("Results: ");
        Serial.print("   ");
        Serial.println(newLimits.max);
        Serial.print("   ");
        Serial.println(newLimits.min);
    }
}

namespace interpretCommand
{
    namespace print
    {
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

            Serial.println("Normal");
            Serial.print("  MAX: ");
            Serial.println(normal.getMax());
            Serial.print("  MIN: ");
            Serial.println(normal.getMin());

            Serial.println("noFilter");
            Serial.print("  MAX: ");
            Serial.println(noFilter.getMax());
            Serial.print("  MIN: ");
            Serial.println(noFilter.getMin());

            Serial.println("faultFan1");
            Serial.print("  MAX: ");
            Serial.println(faultFan1.getMax());
            Serial.print("  MIN: ");
            Serial.println(faultFan1.getMin());
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
        Serial.println("All data cleared!");
    }
    void toggleSerialEnable()
    {
        serialEnabled = !serialEnabled;
    }
    void recalculateFromNormal()
    {
        int base;
        int A = 27;
        int B = 39;
        int C = 56;
        int D = 69;
        StatData newLimits = tachoAnalysis(1000, 20);
        newLimits.max += 5;
        newLimits.min -= 5;
        normal.setLimit(newLimits);
        putData(newLimits, data, "normalLimits");
        base = newLimits.min;

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

using namespace interpretCommand;
using namespace interpretCommand::print;
Interpreter printTachoValue("tacho", tachoValue);
Interpreter printPeriferial("periferial", periferialValue);
Interpreter printLimits("limit", limitValue);
Interpreter analyseNoAir("analyse noAir", refreshNoAirLimit);
Interpreter analyseNormal("analyse normal", refreshNormalLimit);
Interpreter analysenoFilter("analyse noFilter", refreshNoFilterLimits);
Interpreter analysefaultFan1("analyse faultFan1", refreshFaultFan1Limits);
Interpreter clearLimits("clear", clearAllData);
Interpreter toggleSerial("enable", toggleSerialEnable);
Interpreter doRecalculation("recalc", recalculateFromNormal);