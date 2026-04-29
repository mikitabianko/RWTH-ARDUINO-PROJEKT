#ifndef IDIGITALBUTTON_H
#define IDIGITALBUTTON_H

#include <Arduino.h>

namespace Drivers {
    class IDigitalButton {
    public:
        virtual ~IDigitalButton() {}

        virtual void init() = 0;
        virtual void read() = 0;

        virtual bool isPressed() = 0;
        virtual bool isReleased() = 0;
        virtual bool isHeld() = 0;
    };
}

#endif