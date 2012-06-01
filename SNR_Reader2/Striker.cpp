#include "Striker.h"
#include "Arduino.h"


Striker::Striker(int pin) :_pin(pin), striker(LOCKED)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  striker=LOCKED;
  lockDuration=5000;
}

void Striker::lock(){
     digitalWrite(_pin, LOW);
     striker = LOCKED;
     Serial.println("STRIKER LOCKED");
}

void Striker::unlock(){
      digitalWrite(_pin, HIGH);
      strikerOpenedTime = millis();
      striker = UNLOCKED;
      Serial.println("STRIKER UNLOCKED");
}

void Striker::check(){
  switch(striker){
    case LOCKED:
     break;
    case UNLOCKED:
      if((millis() - strikerOpenedTime) > lockDuration){
        lock();
      }
      break;
  }
}
