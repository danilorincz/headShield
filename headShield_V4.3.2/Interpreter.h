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
