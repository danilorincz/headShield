#pragma once

namespace interpreter
{
    String command = "";
    String getCommand()
    {
        static String newCommand = "";

        if (Serial.available())
            newCommand = "";
        else
            return newCommand;
        int counter = 0;
        while (Serial.available())
        {
            if (counter > 64)
                return newCommand;
            counter++;
            char incomingChar = Serial.read();
            if (incomingChar != '\n')
            {
                newCommand += incomingChar;
            }
            else
            {
                return newCommand;
            }
        }
    }

}

class Interpreter
{
    typedef void (*Func)(); // Define a function pointer type for convenience
    Func function;
    String command;

public:
    Interpreter(String command, Func function) : command(command), function(function) {}

    void refresh(String &inputCommand)
    {
        String baseCommand;
        String subCommand;
        int delimiterPosition = inputCommand.indexOf(':');

        if (delimiterPosition == -1)
        {
            baseCommand = inputCommand;
        }
        else
        {
            baseCommand = inputCommand.substring(0, delimiterPosition);
        }

        if (this->command.equals(baseCommand))
        {
            if (delimiterPosition == -1)
            {
                function();
                inputCommand = "";
            }
            else
            {
                subCommand = inputCommand.substring(delimiterPosition + 1);
                if (subCommand == "rep")
                {
                    function();
                }
            }
        }
    }
};
/*
switch (serialInput)
{
case 'O':

    break;
case 'T':
    static int prevAverage;
    static int prevMax;
    static int prevMin;
    static int counter;

    if (latestTachoStat.average != prevAverage || latestTachoStat.min != prevMin || latestTachoStat.max != prevMax)
    {
        prevAverage = latestTachoStat.average;
        prevMax = latestTachoStat.max;
        prevMin = latestTachoStat.min;
        //Serial.print("MEASUREMENT NUMBER: ");
        //Serial.println(counter);
        //Serial.print("Ave: ");
        //Serial.println(latestTachoStat.average);
        //Serial.print("Max: ");
        Serial.println(latestTachoStat.max);
        //Serial.print("Min: ");
        Serial.println(latestTachoStat.min);
        //Serial.print("Range size: ");
        //Serial.println(latestTachoStat.max - latestTachoStat.min);

        counter++;
    }
    break;
case 'B':
    Serial.println(battery.percent);
    break;
case 'H':
    Serial.println(headSensor.state);
    break;
case 'V':
    Serial.println(visor.state);
    break;
case 'E':
    serialEnabled = true;
    break;

case 'A':
    performTachoAnalysis(5); // Not put any value in the flash memory
    serialInput = 'X';
    break;
case 'N':
    performTachoAnalysis(1); // Use measurements for normal
    serialInput = 'X';
    break;
case 'I':
    performTachoAnalysis(2); // Use measurements for noFilter
    serialInput = 'X';
    break;
case 'Q':
    performTachoAnalysis(3); // Use measurements for faultFan1
    serialInput = 'X';
    break;
case 'W':
    Serial.println("Normal");
    Serial.print("MAX: ");
    Serial.println(normal.getMax());
    Serial.print("MIN: ");
    Serial.println(normal.getMin());

    Serial.println("noFilter");
    Serial.print("MAX: ");
    Serial.println(noFilter.getMax());
    Serial.print("MIN: ");
    Serial.println(noFilter.getMin());

    Serial.println("faultFan1");
    Serial.print("MAX: ");
    Serial.println(faultFan1.getMax());
    Serial.print("MIN: ");
    Serial.println(faultFan1.getMin());

    Serial.println("notEnoughAirflow");
    Serial.print("MAX: ");
    Serial.println(notEnoughAirflow.getMax());
    Serial.print("MIN: ");
    Serial.println(notEnoughAirflow.getMin());
    serialInput = 'X';
    break;

case 'X':
    serialEnabled = false;
    break;
}
*/
