#ifndef IANALOGBUTTONS_H
#define IANALOGBUTTONS_H

#include <Arduino.h>

namespace Drivers {
    struct ButtonThreshold {
        int min;
        int max;
    };

    class IAnalogButtons {
    public:
        virtual ~IAnalogButtons() {}

        virtual void init() = 0;
        virtual void read() = 0;

        virtual bool isPressed(uint8_t index) const = 0;
        virtual bool isReleased(uint8_t index) const = 0;
        virtual bool isHeld(uint8_t index) const = 0;

        virtual int getRawValue() const = 0;
        virtual uint8_t getButtonCount() const = 0;
    };
}

#endif