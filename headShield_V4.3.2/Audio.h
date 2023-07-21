#pragma once
#include "Device.h"

class Audio : public Device
{
public:
    Audio(int pin) : Device(pin) {}
};
