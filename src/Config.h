#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "drivers/analogButtons/AnalogButtons_Driver.h"

namespace Config {
    namespace Display {
        constexpr uint8_t Width = 128;
        constexpr uint8_t Height = 64;
        constexpr uint8_t Address = 0x3C;
    }

    namespace Pins {
        constexpr int JoystickX = A1;
        constexpr int JoystickY = A0;
        constexpr int JoystickBtn = 10;
        constexpr int AnalogButtons = A2;
    }

    namespace Thresholds {
        constexpr Drivers::ButtonThreshold analogButtons[5] = {
            {150, 250},    
            {350, 450},
            {550, 650},
            {750, 900},
            {950, 1024}
        };
    }

    namespace Timing {
        constexpr unsigned long Debounce = 40;
        constexpr unsigned long UpdateRate = 100;
    }
}

#endif