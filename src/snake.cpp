#include "Snake.h"
#include <deque>

namespace Snake {
    struct Point {
        byte X, Y;
    };
    struct Vector {
        byte X, Y;
    };

    Vector dir = { 0, 0 };

    Point head;
    Point food;
    std::deque<Point>* snake = nullptr;
    unsigned long long* gameMap = nullptr; // flattened 28*2 array for 64x28 bits
    int size;
    bool isAlive;

    inline void bitSetTo(unsigned long long& number, byte n, bool x) {
        number = (number & ~(1ULL << n)) | ((unsigned long long)x << n);
    }

    void setMap(Point a, bool x) {
        bitSetTo(gameMap[a.Y * 2 + (a.X / 32)], a.X % 32, x);
    }
    bool getMap(Point a) {
        return (gameMap[a.Y * 2 + (a.X / 32)] & (1ULL << (a.X % 32))) != 0;
    }

    void generate_food() {
        Point p;
        do {
            p.X = rand() % 62 + 1;
            p.Y = rand() % 26 + 1;
        } while (getMap(p));
        food = p;
    }

    void init() {
        Runtime::updatePeriod = 100;
        Runtime::showPeriod = 100;

        dir = {0, 0};

        if (gameMap == nullptr) {
            gameMap = new unsigned long long[28 * 2];
        }
        if (snake == nullptr) {
            snake = new std::deque<Point>();
        }

        for (int i = 0; i < 28 * 2; ++i) gameMap[i] = 0;
        for (int i = 0; i < 28; ++i) {
            setMap({0, i}, 1);
            setMap({63, i}, 1);
        }
        for (int i = 0; i < 64; ++i) {
            setMap({i, 0}, 1);
            setMap({i, 27}, 1);
        }

        head = { 32, 14 };
        snake->clear();
        snake->push_back(head);
        setMap(head, 1);
        size = 1;
        isAlive = true;

        generate_food();
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

        if (isAlive) {
            System::display.setTextSize(1);
            System::display.setTextColor(SH110X_WHITE);
            System::display.setCursor(3, 0);
            System::display.print("Score: ");
            System::display.print(size);

            for (int i = 0; i < 28; ++i) {
                for (int j = 0; j < 64; ++j) {
                    if (getMap({j, i})) {
                        System::display.drawPixel(2 * j, 8 + 2 * i, SH110X_WHITE);
                        System::display.drawPixel(2 * j + 1, 8 + 2 * i, SH110X_WHITE);
                        System::display.drawPixel(2 * j, 8 + 2 * i + 1, SH110X_WHITE);
                        System::display.drawPixel(2 * j + 1, 8 + 2 * i + 1, SH110X_WHITE);
                    }
                }
            }

            // Draw food
            int fx = 2 * food.X;
            int fy = 8 + 2 * food.Y;
            System::display.drawPixel(fx, fy, SH110X_WHITE);
            System::display.drawPixel(fx + 1, fy, SH110X_WHITE);
            System::display.drawPixel(fx, fy + 1, SH110X_WHITE);
            System::display.drawPixel(fx + 1, fy + 1, SH110X_WHITE);
        } else {
            drawCenteredText("Your score: " + String(size), 0, 1, SH110X_WHITE);
            drawCenteredText("Game Over!", 26, 2, SH110X_WHITE);
            drawCenteredText("Press to play again", 56, 1, SH110X_WHITE);
        }

        System::display.display();
    }

    int normalize(int n) {
        if (n > 650) return -1;
        if (n < 350) return 1;
        return 0;
    }

    bool check(Point a) {
        return getMap(a);
    }

    void kill() {
        isAlive = false;
        dir = {0, 0};
    }

    void shift() {
        if (dir.X == 0 && dir.Y == 0) return;

        Point new_head = { static_cast<byte>(head.X + dir.X), static_cast<byte>(head.Y + dir.Y) };
        if (check(new_head)) {
            kill();
            return;
        }

        setMap(new_head, 1);
        snake->push_front(new_head);
        head = new_head;

        bool ate = (new_head.X == food.X && new_head.Y == food.Y);
        if (ate) {
            generate_food();
        } else {
            Point old_tail = snake->back();
            snake->pop_back();
            setMap(old_tail, 0);
        }

        size = snake->size();
    }

    void update() {
        if (System::input.analogButtons[0].held) {
            Runtime::closeApp();
        }

        if (!isAlive) {
            if (System::input.joystickButton.held) init();
            return;
        }

        byte x = 0;
        byte y = 0;

        if (System::input.joystick.up.entered) x = 1;
        else if (System::input.joystick.down.entered) x = -1;
        else if (System::input.joystick.left.entered) y = -1;
        else if (System::input.joystick.right.entered) y = 1;

        if (System::input.analogButtons[1].held) x = 1;
        if (System::input.analogButtons[2].held) x = -1;
        if (System::input.analogButtons[3].held) y = -1;
        if (System::input.analogButtons[4].held) y = 1;

        if (abs(x) != abs(y)) {
            if ((dir.X != -y && dir.Y != x) || (dir.X == 0 && dir.Y == 0))
                dir = {y, -x};
        }
        if (dir.X == dir.Y && dir.X != 0) dir.X = 0;

        shift();

        Runtime::wasUpdate = true;
    }

    void clean() {
        if (snake != nullptr) {
            delete snake;
            snake = nullptr;
        }
        if (gameMap != nullptr) {
            delete[] gameMap;
            gameMap = nullptr;
        }
    }
}