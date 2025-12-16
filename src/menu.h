#ifndef MENU_H
#define MENU_H
#include "Arduino.h"
#include <Adafruit_SH110X.h>
#include "game.h"

extern const int gamesCount;
extern Game games[];
extern int gameToRun;

void drawCenteredText(Adafruit_SH1106G& display, int SCREEN_W, String& text, int y, uint8_t size, uint8_t color);
void drawMenu(Adafruit_SH1106G& display, int SCREEN_H, int SCREEN_W);
void processMenu(int joystickX, int joystickY, int joystickButtonState);
#endif