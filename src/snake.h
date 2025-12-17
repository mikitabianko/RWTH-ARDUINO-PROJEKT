#ifndef SNAKE_H
#define SNAKE_H

#include "Arduino.h"
#include <Adafruit_SH110X.h>

namespace Snake {
    void init();
    void draw();
    void update();
    byte getGameState();
    void clean();
}

#endif