#ifndef Pot_h
#define Pot_h
#include "Arduino.h"
#define NUM_READINGS 8

class Pot
{
private:
    int16_t readings[NUM_READINGS];
    uint8_t index;
    int16_t total;
    uint8_t pin;
    int16_t reading;
    int16_t lastReading;

public:
    void begin(uint8_t pin_)
    {
        pin = pin_;
        for (uint8_t i = 0; i < NUM_READINGS; i++)
        {
            readings[index] = 0;
        }
    }

    bool update()
    {
        total -= readings[index];
        analogRead(pin);
        readings[index] = analogRead(pin);
        total += readings[index];
        index = (index + 1) % NUM_READINGS;
        reading = total / NUM_READINGS;
        if (abs(reading - lastReading) > 31)
        {
            lastReading = reading;
            return true;
        }
        else
        {
            return false;
        }
    }

    uint8_t getValue()
    {
        return reading >> 5;
    }
};
#endif