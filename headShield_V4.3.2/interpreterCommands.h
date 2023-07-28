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
                if (dataNow.average < allTimeMin)
                    allTimeMin = dataNow.average;
                if (dataNow.average > allTimeMax)
                    allTimeMax = dataNow.average;
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
    case cond::noAir:
        oldLimitsClone = noAir.getStatData();
        break;
    case cond::normal:
        oldLimitsClone = normal.getStatData();
        break;
    case cond::noFilter:
        oldLimitsClone = noFilter.getStatData();
        break;
    case cond::faultFan1:
        oldLimitsClone = faultFan1.getStatData();
        break;
    case cond::noAirSingle:
        oldLimitsClone = noAirSingle.getStatData();
        break;
    case cond::noFilterSingle:
        oldLimitsClone = noFilterSingle.getStatData();
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
        case cond::noAir:
            putData(newLimits, data, "noAir");
            noAir.setLimit(newLimits);
            break;
        case cond::normal:
            putData(newLimits, data, "normal");
            normal.setLimit(newLimits);
            break;
        case cond::noFilter:
            putData(newLimits, data, "noFilter");
            noFilter.setLimit(newLimits);
            break;
        case cond::faultFan1:
            putData(newLimits, data, "faultFan1");
            faultFan1.setLimit(newLimits);
            break;
        case cond::noAirSingle:
            putData(newLimits, data, "noAirSingle");
            noAirSingle.setLimit(newLimits);
            break;
        case cond::noFilterSingle:
            putData(newLimits, data, "noFilterSingle");
            noFilterSingle.setLimit(newLimits);
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

            Serial.println("noAirSingle");
            Serial.print("  MAX: ");
            Serial.println(noAirSingle.getMax());
            Serial.print("  MIN: ");
            Serial.println(noAirSingle.getMin());
            Serial.print("  AVE: ");
            Serial.println(noAirSingle.getAverage());

            Serial.println("Normal");
            Serial.print("  MAX: ");
            Serial.println(normal.getMax());
            Serial.print("  MIN: ");
            Serial.println(normal.getMin());
            Serial.print("  AVE: ");
            Serial.println(normal.getAverage());

            Serial.println("noFilterSingle");
            Serial.print("  MAX: ");
            Serial.println(noFilterSingle.getMax());
            Serial.print("  MIN: ");
            Serial.println(noFilterSingle.getMin());
            Serial.print("  AVE: ");
            Serial.println(noFilterSingle.getAverage());

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
        void printNakedLimits()
        {
            Serial.println("Bare limit values (NORMAL, NO_AIR_SINGLE, NO_FILTER, ");

            Serial.println(noAir.getAverage());
            Serial.println(noAirSingle.getAverage());
            Serial.println(normal.getAverage());
            Serial.println(noFilterSingle.getAverage());
            Serial.println(noFilter.getAverage());
            Serial.println(faultFan1.getAverage());
            Serial.println("");

            Serial.println(noAir.getMax());
            Serial.println(noAir.getMin());
            Serial.println(noAirSingle.getMax());
            Serial.println(noAirSingle.getMin());
            Serial.println(normal.getMax());
            Serial.println(normal.getAverage());
            Serial.println(normal.getMin());
            Serial.println(noFilterSingle.getMax());
            Serial.println(noFilterSingle.getMin());
            Serial.println(noFilter.getMax());
            Serial.println(noFilter.getMin());
            Serial.println(faultFan1.getMax());
            Serial.println(faultFan1.getMin());
        }
    }
    void refresh_noAir()
    {
        refreshLimits(cond::noAir);
    }
    void refresh_normal()
    {
        refreshLimits(cond::normal);
    }
    void refresh_noFilter()
    {
        refreshLimits(cond::noFilter);
    }
    void refresh_faultFan1()
    {
        refreshLimits(cond::faultFan1);
    }
    void refresh_noAirSingle()
    {
        refreshLimits(cond::noAirSingle);
    }
    void refresh_noFilterSingle()
    {
        refreshLimits(cond::noFilterSingle);
    }
    void clearAllData()
    {
        nvs_flash_erase();
        nvs_flash_init();
        noAir.clear();
        noAirSingle.clear();
        normal.clear();
        noFilterSingle.clear();
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
        int P_4 = 39;
        int P_3 = 29;
        int P_2 = 26;
        int P_1 = 18;

        int base;

        int N_1 = 30;
        int N_2 = 38;
        int N_3 = 65;
        int N_4 = 69;
        int N_5 = 93;
        int N_6 = 95;

        StatData newLimits = tachoAnalysis(400, 20);

        base = newLimits.average;

        noAir.setMax(base + P_4);
        noAir.setMin(base + P_3);
        noAirSingle.setMax(base + P_2);
        noAirSingle.setMin(base + P_1);

        normal.setLimit(newLimits);

        noFilterSingle.setMax(base - N_1);
        noFilterSingle.setMin(base - N_2);
        noFilter.setMax(base - N_3);
        noFilter.setMin(base - N_4);
        faultFan1.setMax(base - N_5);
        faultFan1.setMin(base - N_6);

        putData(noAir.getStatData(), data, "noAir");
        putData(noAirSingle.getStatData(), data, "noAirSingle");

        putData(normal.getStatData(), data, "normal");

        putData(noFilterSingle.getStatData(), data, "noFilterSingle");
        putData(noFilter.getStatData(), data, "noFilter");
        putData(faultFan1.getStatData(), data, "faultFan1");

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
Interpreter printBareLimits("limit", printNakedLimits);

using namespace interpretCommand;
Interpreter analyse_noAir("noAir", refresh_noAir);
Interpreter analyse_noAirSingle("noAirSingle", refresh_noAirSingle);

Interpreter analyse_normal("normal", refresh_normal);

Interpreter analyse_noFilterSingle("noFilterSingle", refresh_noFilterSingle);
Interpreter analyse_noFilter("noFilter", refresh_noFilter);
Interpreter analyse_faultFan1("faultFan1", refresh_faultFan1);

Interpreter clearLimits("clear", clearAllData);
Interpreter toggleSerial("enable", toggleSerialEnable);
Interpreter doRecalculation("recalc", recalculateFromNormal);