#include "AnalogButtons_Driver.h"

namespace Drivers {
    AnalogButtons_Driver::AnalogButtons_Driver(int p, const ButtonThreshold* thresh, uint8_t count, unsigned long delay)
        : pin(p), thresholds(const_cast<ButtonThreshold*>(thresh)), buttonCount(count), debounceDelay(delay) {
        
        currentState     = 0;
        lastReading      = 0;
        _isPressed       = 0;
        _isReleased      = 0;
    }

    AnalogButtons_Driver::~AnalogButtons_Driver() {}

    void AnalogButtons_Driver::init() {
        AnalogButtons_Driver::read();
    }

    void AnalogButtons_Driver::read() {
        if ((millis() - lastDebounceTime) < debounceDelay) return;
        _isReleased = 0;
        _isPressed = 0;

        int analogValue = analogRead(pin);

        if (abs(lastAnalogValue - analogValue) < 50) return;
        lastAnalogValue = analogValue;  

        int activeButton = -1;  
        for (uint8_t i = 0; i < buttonCount; i++) {
            if (analogValue >= thresholds[i].min && analogValue <= thresholds[i].max) {
                activeButton = i;
                break;  
            }
        }

        byte newState = 0;
        if (activeButton >= 0) {
            newState = (1 << activeButton);
        }

        if (newState != currentState) {
            byte previousState = currentState;
            currentState = newState;

            _isPressed = currentState & ~previousState;   
            _isReleased = previousState & ~currentState; 
        }

        if (newState != currentState) {
            lastDebounceTime = millis();
        }
    }

    bool AnalogButtons_Driver::isPressed(uint8_t index) const {
        if (index >= buttonCount) return false;
        return (_isPressed >> index) & 1;
    }

    bool AnalogButtons_Driver::isReleased(uint8_t index) const {
        if (index >= buttonCount) return false;
        return (_isReleased >> index) & 1;
    }

    bool AnalogButtons_Driver::isHeld(uint8_t index) const {
        if (index >= buttonCount) return false;
        return (currentState >> index) & 1;
    }

    int AnalogButtons_Driver::getRawValue() const {
        return lastAnalogValue;
    }

    uint8_t AnalogButtons_Driver::getButtonCount() const {
        return buttonCount;
    }
}