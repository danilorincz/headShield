#pragma once

class Beeper
{
public:
  int piezoPin;
  int channel;
  bool key;
  Beeper(int piezoPin, int channel, bool key)
  {
    this->piezoPin = piezoPin;
    this->channel = channel;
    this->key = key;
  }
  void begin()
  {
    pinMode(piezoPin, OUTPUT);
  }
  void boot()
  {
    if (key)
    {

      tone(piezoPin, NOTE_A4, 100, channel);
      tone(piezoPin, NOTE_B4, 100, channel);
      tone(piezoPin, NOTE_C4, 100, channel);
      tone(piezoPin, NOTE_D4, 100, channel);
      tone(piezoPin, NOTE_E4, 100, channel);
      tone(piezoPin, NOTE_F4, 100, channel);
    }
  }
  void visorDown()
  {
    if (key)
      tone(piezoPin, NOTE_F4, 100, channel);
  }
  void visorUp()
  {
    if (key)
      tone(piezoPin, NOTE_A4, 100, channel);
  }
  void fanOn()
  {
    if (key)
      tone(piezoPin, NOTE_C4, 100, channel);
  }
  void fanOff()
  {
    if (key)
      tone(piezoPin, NOTE_E4, 100, channel);
  }
  void lampIncriase()
  {
    if (key)
    {
      tone(piezoPin, NOTE_A4, 100, channel);
      tone(piezoPin, NOTE_C4, 100, channel);
    }
  }
  void lampOff()
  {
    if (key)
    {
      tone(piezoPin, NOTE_C4, 100, channel);
      tone(piezoPin, NOTE_A4, 100, channel);
    }
  }
};
