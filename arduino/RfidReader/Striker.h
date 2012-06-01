#ifndef Striker_h
#define Striker_h

#include "Arduino.h"

class Striker
{
  public:
    Striker(int pin);
    void lock();
    void unlock();
    void check();
  private:
    int _pin;
    enum strikerStates {LOCKED, UNLOCKED};
    unsigned long strikerOpenedTime;
    unsigned long lockDuration;
    strikerStates striker;
};

#endif
