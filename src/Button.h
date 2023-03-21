#ifndef button_h
#define button_h
#define MASK 0b11000111

#include "Arduino.h"

class Button
{
private:
    uint8_t pin;
    uint8_t history;

public:
    void begin(uint8_t pin_)
    {
        pin = pin_;
        history = 0;
        pinMode(pin, INPUT_PULLUP);
    }

    void update()
    {
        history = (history << 1);
        history |= digitalRead(pin);
    }

    bool isPressed() // was isReleased
    {
        bool released = false;
        if ((history & MASK) == 0b11000000)
        {
            released = true;
            history = 0b00000000;
        }
        return released;
    }
    
    bool isReleased() // was isPressed
    {
        bool pressed = false;
        if ((history & MASK) == 0b00000111)
        {
            pressed = true;
            history = 0b11111111;
        }
        return pressed;
    }
};


#endif