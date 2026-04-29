#include "Dino.h"

namespace Dino {
    struct Obstacle {
        int x;
        int type; // 0: cactus
    };

    const int maxObstacles = 5;
    Obstacle* obstacles = nullptr;
    int numObstacles;
    int dinoY; 
    int dinoVel;
    bool isAlive;
    int score;
    int groundSpeed;
    int nextObsCounter;
    const int groundY = 45;
    const int dinoX = 10;
    const int dinoWidth = 16;
    const int dinoHeight = 16;
    const int cactusWidth = 8;
    const int cactusHeight = 16;

    const uint8_t dinoBitmap[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xFE, 0xFE, 0x7C, 0x38,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }; 

    const uint8_t actualDinoBitmap[32] = { 
        0b00000000, 0b00000000,
        0b00000000, 0b00000000,
        0b00000111, 0b11111110,
        0b00001000, 0b00000001,
        0b00001011, 0b10000001,
        0b00001010, 0b10000001,
        0b00001011, 0b10000001,
        0b11001000, 0b00000001,
        0b10111000, 0b00000001,
        0b10000000, 0b00000010,
        0b01000000, 0b00001100,
        0b01000000, 0b00010000,
        0b01000000, 0b00100000,
        0b00110111, 0b00100000,
        0b00011000, 0b11000000,
        0b00010000, 0b10000000
    }; // Rough T-rex shape: adjust as needed

    const uint8_t cactusBitmap[16] = { // 8x16, 16 bytes
        0b00000000,
        0b00000000,
        0b00011000,
        0b00011000,
        0b00011011,
        0b00011011,
        0b11011011,
        0b11011110,
        0b11011100,
        0b01111000,
        0b01111000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000
    }; 

    void init() {
        Runtime::updatePeriod = 50;
        Runtime::showPeriod = 50;

        if (obstacles == nullptr) {
            obstacles = new Obstacle[maxObstacles];
        }
        numObstacles = 0;

        dinoY = 0;
        dinoVel = 0;
        isAlive = true;
        score = 0;
        groundSpeed = 4;
        nextObsCounter = 30 + rand() % 20; 
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
            // Draw ground
            System::display->drawLine(0, groundY, 128, groundY, Drivers::Color::White);

            // Draw dino
            int dinoBottomY = groundY - dinoY - dinoHeight + 1;
            System::display->drawBitmap(dinoX, dinoBottomY, actualDinoBitmap, dinoWidth, dinoHeight, Drivers::Color::White);

            // Draw obstacles
            for (int i = 0; i < numObstacles; ++i) {
                if (obstacles[i].type == 0) {
                    System::display->drawBitmap(obstacles[i].x, groundY - cactusHeight + 1, cactusBitmap, cactusWidth, cactusHeight, Drivers::Color::White);
                }
            }

            // Draw score
            System::display->setTextSize(1);
            System::display->setTextColor(Drivers::Color::White);
            System::display->setCursor(0, 0);
            System::display->print("HI ");
            System::display->print(score);
        } else {
            drawCenteredText("Game Over!", 15, 2, Drivers::Color::White);
            drawCenteredText("Score: " + String(score), 35, 1, Drivers::Color::White);
            drawCenteredText("Press to restart", 50, 1, Drivers::Color::White);
        }

        System::display->display();
    }

    void update() {
        if (System::input.analogButtons[0].released) {
            Runtime::closeApp();
        }

        if (!isAlive) {
            if (System::input.joystickButton.released) {
                init();
            }
            return;
        }

        // Handle jump
        if (dinoY == 0 && (System::input.joystick.lastUpdated == System::Direction::Up || System::input.analogButtons[1].released)) {
            dinoVel = 12; // lower jump
        }

        // Update dino physics with stronger gravity
        dinoY += dinoVel;
        if (dinoY > 0) {
            dinoVel -= 3; // stronger gravity for faster land
        }
        if (dinoY < 0) {
            dinoY = 0;
            dinoVel = 0;
        }

        // Update speed over time
        groundSpeed = 6 + score / 300;

        // Move obstacles
        for (int i = 0; i < numObstacles; ++i) {
            obstacles[i].x -= groundSpeed;
        }

        // Remove off-screen obstacles
        while (numObstacles > 0 && obstacles[0].x < -cactusWidth) {
            for (int i = 1; i < numObstacles; ++i) {
                obstacles[i-1] = obstacles[i];
            }
            --numObstacles;
        }

        // Generate new obstacle
        --nextObsCounter;
        if (nextObsCounter <= 0 && numObstacles < maxObstacles) {
            Obstacle obs;
            obs.x = 128;
            obs.type = 0;
            obstacles[numObstacles++] = obs;
            nextObsCounter = 20 + rand() % 30; // random frames between obstacles
        }

        // Check collisions
        int dinoRight = dinoX + dinoWidth;
        int dinoTop = groundY - dinoY - dinoHeight;
        int dinoBottom = groundY - dinoY;
        for (int i = 0; i < numObstacles; ++i) {
            int obsLeft = obstacles[i].x;
            int obsRight = obstacles[i].x + cactusWidth;
            int obsTop = groundY - cactusHeight;
            int obsBottom = groundY;
            if (dinoRight > obsLeft && dinoX < obsRight &&
                dinoBottom > obsTop && dinoTop < obsBottom) {
                isAlive = false;
                break;
            }
        }

        // Increment score
        score += groundSpeed / 2; // adjust scoring

        Runtime::wasUpdate = true;
    }

    void clean() {
        if (obstacles != nullptr) {
            delete[] obstacles;
            obstacles = nullptr;
        }
    }
}