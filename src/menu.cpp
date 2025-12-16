#include "Arduino.h"
#include "menu.h"
#include <Adafruit_SH110X.h>

int selectedGame = 0;
int gameToRun = -1;

int normalize(int n) {
    if (n > 700) return -1;
    if (n < 300) return 1;
    return 0;
}

void drawCenteredText(Adafruit_SH1106G& display, int screen_w, String& text, int y, uint8_t size, uint8_t color) {
    int16_t x1, y1;
    uint16_t w, h;

    display.setTextSize(size);
    display.setTextColor(SH110X_WHITE);
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    int x = (screen_w - w) / 2;
    display.setCursor(x, y);
    display.print(text);
}

bool isMenuMode = true;
void drawMenu(Adafruit_SH1106G& display, int screen_h, int screen_w) {
    display.clearDisplay();
    int page = selectedGame / 3;
    for (int i = 0; i < min(3, gamesCount - page * 3); ++i) {        
        int newId = page * 3 + i;

        if (selectedGame == newId) 
            drawCenteredText(display, screen_w, games[newId].Name, screen_h / 3 * i + 4, 2, SH110X_WHITE);
        else 
            drawCenteredText(display, screen_w, games[newId].Name, screen_h / 3 * i + 8, 1, SH110X_WHITE);
    }
    display.display();
}

void processMenu(int joystickX, int joystickY, int joystickButtonState) {
    selectedGame = (selectedGame - normalize(joystickX) + gamesCount) % gamesCount;
    if (joystickButtonState == LOW) {
        gameToRun = selectedGame;
    }
}