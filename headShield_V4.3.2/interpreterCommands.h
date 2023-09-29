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

        if (fan.getCurrentSessionOn() > spinUpTime)
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
        void printAdaptiveSettings()
        {
            // Load settings from flash
            loadAdaptiveSettings(data, ADAPT);

            // Print settings to Serial
            Serial.println("Current Settings:");
            Serial.print("ABSOLUTE_MIN: ");
            Serial.println(ADAPT.ABSOLUTE_MIN);
            Serial.print("NOFILTER_CONST_MAX: ");
            Serial.println(ADAPT.NOFILTER_CONST_MAX);
            Serial.print("NORMAL_INITIAL_MIN: ");
            Serial.println(ADAPT.NORMAL_INITIAL_MIN);
            Serial.print("NORMAL_CONST_MIN: ");
            Serial.println(ADAPT.NORMAL_CONST_MIN);
            Serial.print("NORMAL_CONST_MAX: ");
            Serial.println(ADAPT.NORMAL_CONST_MAX);
            Serial.print("NORMAL_INITIAL_MAX: ");
            Serial.println(ADAPT.NORMAL_INITIAL_MAX);
            Serial.print("NOAIR_CONST_MIN: ");
            Serial.println(ADAPT.NOAIR_CONST_MIN);
            Serial.print("ABSOLUTE_MAX: ");
            Serial.println(ADAPT.ABSOLUTE_MAX);
            Serial.print("LOWER_DIF: ");
            Serial.println(ADAPT.LOWER_DIF);
            Serial.print("UPPER_DIF: ");
            Serial.println(ADAPT.UPPER_DIF);
            Serial.print("MAX_ACCEL: ");
            Serial.println(ADAPT.MAX_ACCEL, 2); // 2 decimal places
        }
        void filterOnTime()
        {
            Serial.println(filterTracker.get_timeOn());
        }
        void memoryWear()
        {
            Serial.println(filterTracker.get_memoryWear());
        }
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
            static int lastValue = 0;
            if (lastValue != tacho.finalValue)
            {
                Serial.println(tacho.finalValue);
                lastValue = tacho.finalValue;
            }
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

    }
    void manualFanToggle()
    {
        fan.toggle();
        if (fan.state)
            Serial.println("Fan ON");
        else
            Serial.println("Fan OFF");
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

    void batteryTest()
    {
        Timer log(30 * 1000);
        fan.on();

        unsigned int realPercent;
        unsigned int lowestPercent = 100;

        unsigned int sinceStartMinute = 0;

        int allTimeMinTacho = 9999;
        int allTimeMaxTacho = 0;

        Serial.println("BATTERY TEST PROGRAM START");
        Serial.println(" ");
        int condition = 0; // 0-> normal 1-> noFilter 2-> noAir
        int prevCondition = -1;
        while (true)
        {

            realPercent = battery.getPercent();

            if (realPercent < lowestPercent)
            {
                lowestPercent = realPercent;
            }

            static Timer runTachoMeasure(5);
            if (runTachoMeasure.timeElapsedMillis())
            {
                tacho.getAverage();
            }

            if (tacho.finalValue < allTimeMinTacho && tacho.finalValue > 3000)
                allTimeMinTacho = tacho.finalValue;

            if (tacho.finalValue > allTimeMaxTacho)
                allTimeMaxTacho = tacho.finalValue;

            if (condition != prevCondition)
            {
                prevCondition = condition;
                Serial.println(getSSID());
                switch (condition)
                {
                case 0:
                    Serial.println("NORMAL");
                    break;
                case 1:
                    Serial.println("NO FILTER");
                    break;
                case 2:
                    Serial.println("NO AIR");
                    break;
                }
                visor.scan();
                if (visor.state)
                    Serial.println("VISOR DOWN");
                else
                    Serial.println("VISOR UP");
                Serial.print("Min.");
                Serial.print("\t");
                Serial.print("Charge");
                Serial.print("\t");
                Serial.print("T");
                Serial.print("\t");
                Serial.print("T_MIN");
                Serial.print("\t");
                Serial.println("T_MAX");
            }

            if (log.timeElapsedMillis())
            {
                sinceStartMinute++;
                float sinceStartMinuteFloat = (float)sinceStartMinute / 2.00;
                Serial.print(sinceStartMinuteFloat);
                Serial.print("\t");
                Serial.print(realPercent);
                Serial.print("\t");
                Serial.print(tacho.finalValue);
                Serial.print("\t");
                Serial.print(allTimeMinTacho);
                Serial.print("\t");
                Serial.println(allTimeMaxTacho);

                allTimeMinTacho = 9999;
                allTimeMaxTacho = 0;
            }

            if (sinceStartMinute == 10)
            {
                sinceStartMinute = 0;
                switch (condition)
                {
                case 0:
                    Serial.println("normal done! remove the filters");
                    break;
                case 1:
                    Serial.println("no filter done! add the clogged filter");
                    break;
                case 2:
                    Serial.println("no air done! change device");
                    return;
                    break;
                }
                condition++;
                Serial.println("Press any button to continue");

                while (true)
                {
                    if (runTachoMeasure.timeElapsedMillis())
                    {
                        tacho.getAverage();
                    }
                    if (Serial.available())
                    {
                        for (int i = 0; i < 64; i++)
                        {
                            int a = Serial.read();
                        }
                        log.preTime = millis();
                        break;
                    }
                }
            }
        }
    }

    void performLongTest()
    {
        Timer log(30 * 1000);
        fan.on();

        unsigned int realPercent;

        unsigned int sinceStartMinute = 0;

        int allTimeMinTacho = 9999;
        int allTimeMaxTacho = 0;

        Serial.println("LONG TEST PROGRAM START");
        Serial.println(" ");

        while (true)
        {
            realPercent = battery.getPercent();

            static Timer runTachoMeasure(5);
            if (runTachoMeasure.timeElapsedMillis())
            {
                tacho.getAverage();
            }

            if (tacho.finalValue < allTimeMinTacho && tacho.finalValue > 3000)
                allTimeMinTacho = tacho.finalValue;

            if (tacho.finalValue > allTimeMaxTacho)
                allTimeMaxTacho = tacho.finalValue;

            if (log.timeElapsedMillis())
            {
                sinceStartMinute++;

                Serial.print(sinceStartMinute);
                Serial.print("\t");
                Serial.print(realPercent);
                Serial.print("\t");
                Serial.print(tacho.finalValue);
                Serial.print("\t");
                Serial.print(allTimeMinTacho);
                Serial.print("\t");
                Serial.println(allTimeMaxTacho);

                allTimeMinTacho = 9999;
                allTimeMaxTacho = 0;
            }

            if (sinceStartMinute == 90)
            {
                fan.off();
                Serial.println("FINISHED______________________________");
                return;
            }
        }
    }
    void enableAccel()
    {
        accelPrintEnable = !accelPrintEnable;
    }

}
using namespace interpretCommand::print;
Interpreter printTachoValue("tacho", tachoValue);
Interpreter printPeriferial("input", inputValues);
Interpreter printLimits("limit", limitValues);
Interpreter printAdaptive("adaptive data", printAdaptiveSettings);

Interpreter printFilterTime("filter", filterOnTime);
Interpreter printMemoryWear("wear", memoryWear);

using namespace interpretCommand;
Interpreter analyse_normal("normal", refreshNormal);
Interpreter clearLimits("clear", clearAllData);
Interpreter toggleSerial("enable serial", toggleSerialEnable);
Interpreter printSensorValues("sensor", sensorValues);

Interpreter toggleFan("fan", manualFanToggle);
Interpreter analyseBattery("battery test", batteryTest);
Interpreter longTest("long test", performLongTest);
Interpreter logAcceleration("accel", enableAccel);
