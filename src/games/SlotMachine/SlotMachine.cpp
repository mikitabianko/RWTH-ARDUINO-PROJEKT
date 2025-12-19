#include "SlotMachine.h"

namespace SlotMachine {
    int credits;
    bool isAlive;
    bool spinning;
    int* reels = nullptr;
    bool* stopped = nullptr;
    int* stop_counters = nullptr;
    const int num_symbols = 4;
    const char* symbols[4] = { "7", "C", "L", "B" };
    const int pays[4] = { 50, 10, 5, 20 };
    int gain = 0;

    void init() {
        Runtime::updatePeriod = 50;
        Runtime::showPeriod = 50;

        if (reels == nullptr) reels = new int[3];
        if (stopped == nullptr) stopped = new bool[3];
        if (stop_counters == nullptr) stop_counters = new int[3];

        gain = 0;
        credits = 10;
        isAlive = true;
        spinning = false;

        for (int i = 0; i < 3; ++i) {
            reels[i] = rand() % num_symbols;
        }
    }

    void drawCenteredText(String text, int y, uint8_t size, uint8_t color) {
        int16_t x1, y1;
        uint16_t w, h;

        System::display.setTextSize(size);
        System::display.setTextColor(SH110X_WHITE);
        System::display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

        int x = (SCREEN_W - w) / 2;
        System::display.setCursor(x, y);
        System::display.print(text);
    }

    void show() {
        System::display.clearDisplay();

        System::display.setTextColor(SH110X_WHITE);

        if (isAlive) {
            System::display.setTextSize(1);
            System::display.setCursor(3, 0);
            System::display.print("Credits: ");
            System::display.print(credits);

            if (gain > 0) {
                System::display.print(" (+");
                System::display.print(gain);
                System::display.print(")");
            }

            System::display.setTextSize(7);
            for (int i = 0; i < 3; ++i) {
                System::display.setCursor(i * 42, 8);
                System::display.print(symbols[reels[i]]);
            }

            if (!spinning) {
                System::display.setTextSize(1);
                System::display.setCursor(30, 56);
                System::display.print("Press to spin");
            }
        } else {
            drawCenteredText("Your credits: " + String(credits), 0, 1, SH110X_WHITE);
            drawCenteredText("Game Over!", 26, 2, SH110X_WHITE);
            drawCenteredText("Press to play again", 56, 1, SH110X_WHITE);
        }

        System::display.display();
    }

    void update() {
        if (System::input.analogButtons[0].released) {
            Runtime::closeApp();
        }

        if (!isAlive) {
            if (System::input.joystickButton.released) init();
            return;
        }

        if (!spinning) {
            if ((System::input.joystickButton.released || System::input.joystick.lastUpdated == System::Direction::Down) && credits > 0) {
                gain = 0;
                credits--;
                spinning = true;
                for (int i = 0; i < 3; ++i) {
                    stopped[i] = false;
                    stop_counters[i] = 20 + i * 10 + (rand() % 10);
                }
            }
        } else {
            bool all_stopped = true;
            for (int i = 0; i < 3; ++i) {
                if (!stopped[i]) {
                    reels[i] = (reels[i] + 1) % num_symbols;
                    stop_counters[i]--;
                    if (stop_counters[i] <= 0) {
                        stopped[i] = true;
                    }
                    all_stopped = false;
                }
            }
            if (all_stopped) {
                spinning = false;
                int identicalCount = (reels[0] == reels[1]) + (reels[1] == reels[2]) + (reels[0] == reels[2]);
                if (identicalCount == 3) {
                    gain = pays[reels[0]];
                    credits += pays[reels[0]];
                } else if (identicalCount == 1) {
                    gain = 1;
                    credits += 1;
                } else {
                    gain = 0;
                }
                if (credits <= 0) {
                    isAlive = false;
                }
            }
        }

        Runtime::wasUpdate = true;
    }

    void clean() {
        if (reels != nullptr) {
            delete[] reels;
            reels = nullptr;
        }
        if (stopped != nullptr) {
            delete[] stopped;
            stopped = nullptr;
        }
        if (stop_counters != nullptr) {
            delete[] stop_counters;
            stop_counters = nullptr;
        }
    }
}