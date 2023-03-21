/*
40 GRIT BLE MIDI controller
Sparkfun ESP 32 Thing Plus
LED resistor: 4.7k
LDR: Vcc -> LDR - > 100k Trimpot -> GND
Softpot: Tip - Vcc, Ring - A4, Sleave - GND; Ring -> 10k -> GND
Channel switch: Vcc -> 10k -> 10k -> 10k -> 10k -> 10k -> 10k -> GND. 

20.03.2023:
- The charging circuit for the LiPoly battery caused the board to release magic
smoke and is now dead. The MCU is still operating fine.
- When SoftPot is connected and not touched the readings stay around 47, when
nothing is connected to the port the reading is stable 0. Probably bad SoftPot
or wireing.
- Button B3 is too close to knobs.
- Buttons need to be replaced with new ones.
- LDR need some calibration.
- The Bluetooth connection needs to be checked
- The LDR circuit may be touching the enclosure.
21.03.2023
- Calibrated LDR.
*/

#include <Arduino.h>
#include <BLEMIDI_Transport.h>
#include <Button.h>
#include <Pot.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>

BLEMIDI_CREATE_INSTANCE("40GRIT", MIDI);

uint8_t const DIGITAL_NUM = 8;
uint8_t const ANALOG_NUM = 10;
uint8_t const LED_PIN = 21;
uint8_t const LED_ON_TIME = 50;
uint32_t const TIME_OUT = 30000;
uint8_t const UPDATE_MS = 5;

uint32_t lastControlsUpdateTime;
uint32_t lastLedOnTime;

Pot analogInputs[ANALOG_NUM];

Button digitalInputs[DIGITAL_NUM];
uint8_t const digital_pins[DIGITAL_NUM] = {5, 18, 19, 16, A5, 22, 23, A0};
uint8_t const analog_pins[ANALOG_NUM] = {A3, A4, A2, 13, 12,
                                         27, 33, 15, 32, 14};
enum digital_inputs { spdt1, spdt2, spdt3, spdt4, b1, b2, b3, jack };
enum analog_inputs { channelSwitch, softPot, p1, p2, p3, p4, p5, p6, p7, p8 };

uint8_t midiChannel = 1;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  pinMode(LED_PIN, OUTPUT);

  for (int i = 0; i < ANALOG_NUM; i++) {
    analogInputs[i].begin(analog_pins[i]);
  }

  for (int i = 0; i < DIGITAL_NUM; i++) {
    digitalInputs[i].begin(digital_pins[i]);
  }
}

void loop() {
  // Check all buttons and pots every 5 ms
  if (millis() > lastControlsUpdateTime + UPDATE_MS) {

    // Update analog inputs
    for (int i = 0; i < ANALOG_NUM; i++) {
      if (analogInputs[i].update()) {
        uint8_t value = analogInputs[i].getValue();
        if (i == channelSwitch) { // MIDI channel switch
          if (value > 0) {
            midiChannel = 6;
          }
          if (value > 15) {
            midiChannel = 5;
          }
          if (value > 26) {
            midiChannel = 4;
          }
          if (value > 42) {
            midiChannel = 3;
          }
          if (value > 65) {
            midiChannel = 2;
          }
          if (value > 125) {
            midiChannel = 1;
          }
        } else if (i == softPot) {
          // Unstable Readings
        } else { // Potentiometers
          MIDI.sendControlChange(i, value, midiChannel);
          lastLedOnTime = millis();
          analogWrite(LED_PIN, value >> 2);
        }
      }
    }

    // Update digital inputs
    for (int i = 0; i < DIGITAL_NUM; i++) {
      digitalInputs[i].update();
      if (digitalInputs[i].isPressed()) { // ON
        MIDI.sendControlChange(i + ANALOG_NUM, 127, midiChannel);
        lastLedOnTime = millis();
        analogWrite(LED_PIN, 255);
      } else if (digitalInputs[i].isReleased()) { // OFF
        MIDI.sendControlChange(i + ANALOG_NUM, 0, midiChannel);
        lastLedOnTime = millis();
        analogWrite(LED_PIN, 255);
      }
    }

    lastControlsUpdateTime = millis();
  }

  // Turn the LED off after 50 mS
  if (millis() > lastLedOnTime + LED_ON_TIME) {
    analogWrite(LED_PIN, 0);
  }
}