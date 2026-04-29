#include "Snake.h"
#include <deque>

namespace Snake {
    struct Point {
        byte X, Y;
    };
    struct Vector {
        int X, Y;
    };

    const byte CELL_SIZE = 4; 
    const byte Y_OFFSET  = 8; 
    const byte MAP_W     = Config::Display::Width / CELL_SIZE;
    const byte MAP_H     = (Config::Display::Height - Y_OFFSET) / CELL_SIZE;

    typedef uint32_t MapType;
    const byte BITS_IN_TYPE = sizeof(MapType) * 8;
    const byte ELEMENTS_PER_ROW = (MAP_W + BITS_IN_TYPE - 1) / BITS_IN_TYPE;

    Vector dir = { 0, 0 };

    Point head;
    Point food;
    std::deque<Point>* snake = nullptr;
    unsigned long long* gameMap = nullptr;
    int size;
    bool isAlive;

    inline void bitSetTo(unsigned long long& number, byte n, bool x) {
        number = (number & ~(1ULL << n)) | ((unsigned long long)x << n);
    }

    void setMap(Point a, bool x) {
        bitSetTo(gameMap[a.Y * ELEMENTS_PER_ROW + (a.X / BITS_IN_TYPE)], a.X % BITS_IN_TYPE, x);
    }

    bool getMap(Point a) {
        return (gameMap[a.Y * ELEMENTS_PER_ROW + (a.X / BITS_IN_TYPE)] & (1UL << (a.X % BITS_IN_TYPE))) != 0;
    }

    void generate_food() {
        Point p;
        do {
            p.X = rand() % (MAP_W - 2) + 1;
            p.Y = rand() % (MAP_H - 2) + 1;
        } while (getMap(p));
        food = p;
    }

    void init() {
        Runtime::updatePeriod = 100;
        Runtime::showPeriod = 100;

        dir = {0, 0};

        int mapArraySize = MAP_H * ELEMENTS_PER_ROW;
        if (gameMap == nullptr) {
            gameMap = new unsigned long long[mapArraySize];
        }
        if (snake == nullptr) {
            snake = new std::deque<Point>();
        }

        for (int i = 0; i < mapArraySize; ++i) gameMap[i] = 0;

        for (byte i = 0; i < MAP_H; ++i) {
            setMap({0, i}, 1);
            setMap((Point){(byte)(MAP_W - 1), i}, 1);
        }
        for (byte i = 0; i < MAP_W; ++i) {
            setMap({i, 0}, 1);
            setMap((Point){i, (byte)(MAP_H - 1)}, 1);
        }

        head = { (byte)(MAP_W / 2), (byte)(MAP_H / 2) };

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

        System::display->setTextSize(size);
        System::display->setTextColor(Drivers::Color::White);
        System::display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

        int x = (SCREEN_W - w) / 2;
        System::display->setCursor(x, y);
        System::display->print(text);
    }

    void show() {
        System::display->clear();

        if (isAlive) {
            System::display->setTextSize(1);
            System::display->setTextColor(Drivers::Color::White);
            System::display->setCursor(3, 0);
            System::display->print("Score: ");
            System::display->print(size);

            for (int i = 0; i < MAP_H; ++i) {
                for (int j = 0; j < MAP_W; ++j) {
                    if (getMap({(byte)j, (byte)i})) {
                        System::display->fillRect(
                            j * CELL_SIZE, 
                            Y_OFFSET + i * CELL_SIZE, 
                            CELL_SIZE, 
                            CELL_SIZE, 
                            Drivers::Color::White
                        );
                    }
                }
            }

            // Draw food
            System::display->fillRect(
                food.X * CELL_SIZE, 
                Y_OFFSET + food.Y * CELL_SIZE, 
                CELL_SIZE, 
                CELL_SIZE, 
                Drivers::Color::White
            );
        } else {
            drawCenteredText("Your score: " + String(size), 0, 1, Drivers::Color::White);
            drawCenteredText("Game Over!", 26, 2, Drivers::Color::White);
            drawCenteredText("Press to play again", 56, 1, Drivers::Color::White);
        }

        System::display->display();
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
        if (System::input.analogButtons[0].released) {
            Runtime::closeApp();
        }

        if (!isAlive) {
            if (System::input.joystickButton.released) init();
            return;
        }

        int x = 0;
        int y = 0;

        if (System::input.joystick.lastUpdated == System::Direction::Up) x = 1;
        else if (System::input.joystick.lastUpdated == System::Direction::Down) x = -1;
        else if (System::input.joystick.lastUpdated == System::Direction::Left) y = -1;
        else if (System::input.joystick.lastUpdated == System::Direction::Right) y = 1;

        if (System::input.analogButtons[1].released) x = 1;
        if (System::input.analogButtons[2].released) x = -1;
        if (System::input.analogButtons[3].released) y = -1;
        if (System::input.analogButtons[4].released) y = 1;

        if (abs(x) != abs(y)) {
            if ((dir.X != -y && dir.Y != x) || (dir.X == 0 && dir.Y == 0))
                dir = {y, -x};
        }

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