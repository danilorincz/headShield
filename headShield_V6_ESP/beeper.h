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
    tone(piezoPin, NOTE_A4, 100, channel);
    tone(piezoPin, NOTE_B4, 100, channel);
    tone(piezoPin, NOTE_C4, 100, channel);
    tone(piezoPin, NOTE_D4, 100, channel);
    tone(piezoPin, NOTE_E4, 100, channel);
    tone(piezoPin, NOTE_F4, 100, channel);
  }
  void visorDown()
  {
    tone(piezoPin, NOTE_F4, 100, channel);
  }
  void visorUp()
  {
    tone(piezoPin, NOTE_A4, 100, channel);
  }
  void fanOn()
  {
    tone(piezoPin, NOTE_C4, 100, channel);
  }
  void fanOff()
  {
    tone(piezoPin, NOTE_E4, 100, channel);
  }
};
