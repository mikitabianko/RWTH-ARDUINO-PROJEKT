#include "Arduino.h"
#include "./DigitalButton_Driver.h"

namespace Drivers {
    DigitalButton_Driver::DigitalButton_Driver(int pin, unsigned long debounce) 
        : _pin(pin), _debounceDelay(debounce) {}

    void DigitalButton_Driver::init() {
        pinMode(_pin, INPUT_PULLUP);
        _lastReading = (digitalRead(_pin) == LOW);
        _currentState = _lastReading;
    }

    void DigitalButton_Driver::read() {
        bool reading = (digitalRead(_pin) == LOW);
        _isPressed = false;
        _isReleased = false;

        if (reading != _lastReading) {
            _lastDebounceTime = millis();
            _lastReading = reading;
        }

        if ((millis() - _lastDebounceTime) > _debounceDelay) {
            if (reading != _currentState) {
                _currentState = reading;
                if (_currentState) _isPressed = true;
                else _isReleased = true;
            }
        }
    }

    bool DigitalButton_Driver::isPressed() { return _isPressed; }
    bool DigitalButton_Driver::isReleased()  { return _isReleased; }
    bool DigitalButton_Driver::isHeld()  { return _currentState; }
}