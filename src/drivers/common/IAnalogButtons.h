#ifndef IANALOGBUTTONS_H
#define IANALOGBUTTONS_H

#include <Arduino.h>

namespace Drivers {
    class IAnalogButtons {
    public:
        virtual ~IAnalogButtons() {}

        virtual void init() = 0;
        virtual void read() = 0;

        virtual bool isPressed() = 0;
        virtual bool isReleased() = 0;
        virtual bool isHeld() = 0;
    };
}

#endif