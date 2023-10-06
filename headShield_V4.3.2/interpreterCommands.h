//* INTERPRETER
#include "basic.h"
namespace interpretCommand
{
    namespace print
    {
        void printAdaptiveSettings()
        {
            // Load settings from flash


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
Interpreter printAdaptive("adaptive", printAdaptiveSettings);
Interpreter printFilterTime("filter", filterOnTime);
Interpreter printSensorValues("sensor", sensorValues);

using namespace interpretCommand;
Interpreter clearFlash("clear flash", clearAllData);
Interpreter toggleSerial("serial", toggleSerialEnable);
Interpreter toggleFan("fan", manualFanToggle);
