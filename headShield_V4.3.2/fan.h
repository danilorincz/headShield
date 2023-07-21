#pragma once
#include "Device.h"

class Fan : public Device
{
public:
    Fan(int pin) : Device(pin) {}
};
