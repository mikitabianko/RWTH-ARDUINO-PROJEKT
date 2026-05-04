#ifndef ANALOGBUTTONS_DRIVER_H
#define ANALOGBUTTONS_DRIVER_H
#include "../common/IAnalogButtons.h"

namespace Drivers {
    class AnalogButtons_Driver : public IAnalogButtons {
    private:
        int pin;
        const ButtonThreshold* thresholds;
        uint8_t buttonCount;
        const unsigned long debounceDelay;

        byte currentState;
        byte lastReading;
        byte _isPressed; 
        byte _isReleased;

        unsigned long lastDebounceTime = 0;
        int lastAnalogValue = 0;

    public:
        AnalogButtons_Driver(int p, const ButtonThreshold* thresh, uint8_t count, unsigned long delay = 50);
        ~AnalogButtons_Driver();

        void init() override;
        void read() override;

        bool isPressed(uint8_t index) const override;
        bool isReleased(uint8_t index) const override;
        bool isHeld(uint8_t index) const override;

        int getRawValue() const override;
        uint8_t getButtonCount() const override;
    };
}

#endif