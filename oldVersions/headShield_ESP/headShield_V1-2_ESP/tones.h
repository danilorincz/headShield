#pragma once

class Beeper
{
public:
  int piezoPin;
  int channel;
  Beeper(int piezoPin, int channel)
  {
    this->piezoPin = piezoPin;
    this->channel = channel;
  }
  void begin()
  {
    pinMode(piezoPin, OUTPUT);
  }
  void boot()
  {
    tone(piezoPin, NOTE_A4, 100, piezoChannel);
    tone(piezoPin, NOTE_B4, 100, piezoChannel);
    tone(piezoPin, NOTE_C4, 100, piezoChannel);
    tone(piezoPin, NOTE_D4, 100, piezoChannel);
    tone(piezoPin, NOTE_E4, 100, piezoChannel);
    tone(piezoPin, NOTE_F4, 100, piezoChannel);
  }
};
