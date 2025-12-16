#ifndef GAME_H
#define GAME_H
#include "Arduino.h"
#include <Adafruit_SH110X.h>

class Game {
public:
    String Name;
    void (*init)();
    void (*draw)(Adafruit_SH1106G& display, int screen_h, int screen_w);
    void (*update)(int joystickX, int joystickY, int joystickButtonState);
    // byte (*getGameState)();

    Game(String name, 
        void (*init)(),
        void (*draw)(Adafruit_SH1106G& display, int screen_h, int screen_w),
        void (*update)(int joystickX, int joystickY, int joystickButtonState)) 
    : Name(name), init(init), draw(draw), update(update) {}
};

#endif