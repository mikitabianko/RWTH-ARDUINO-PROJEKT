#include "Menu.h"

namespace Menu {
    App::App menu("menu", init, drawMenu, processMenu, clear);
    byte selectedGame = 0;

    int normalize(int n) {
        if (n > 700) return -1;
        if (n < 300) return 1;
        return 0;
    }

    void drawCenteredText(String& text, int y, uint8_t size, uint8_t color) {
        int16_t x1, y1;
        uint16_t w, h;

        System::display.setTextSize(size);
        System::display.setTextColor(SH110X_WHITE);
        System::display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

        int x = (SCREEN_W - w) / 2;
        System::display.setCursor(x, y);
        System::display.print(text);
    }

    bool isMenuMode = true;
    void drawMenu() {
        System::display.clearDisplay();
        int page = selectedGame / 3;
        for (int i = 0; i < min(3, gamesCount - page * 3); ++i) {        
            int newId = page * 3 + i;

            if (selectedGame == newId) 
                drawCenteredText(games[newId].Name, SCREEN_H / 3 * i + 4, 2, SH110X_WHITE);
            else 
                drawCenteredText(games[newId].Name, SCREEN_H / 3 * i + 8, 1, SH110X_WHITE);
        }
        System::display.display();
    }

    void processMenu() {
        static byte prevSelectedGame = 0;
        if (System::input.joystick.up.entered || (System::input.joystick.up.held && !System::input.joystick.up.exited)) {
            selectedGame = (selectedGame + gamesCount - 1) % gamesCount;
        }
        if (System::input.joystick.down.entered || (System::input.joystick.down.held && !System::input.joystick.down.exited)) {
            selectedGame = (selectedGame + 1) % gamesCount;
        }
        Runtime::wasUpdate = Runtime::wasUpdate || (selectedGame != prevSelectedGame);
        if (System::input.joystickButton.held) {
            Runtime::setApp(games[selectedGame]);
            Runtime::wasUpdate = true;
        }
        prevSelectedGame = selectedGame;
    }

    void init() {
        Runtime::updatePeriod = 150; 
        Runtime::showPeriod = 50; 
    }

    void clear() {}
}