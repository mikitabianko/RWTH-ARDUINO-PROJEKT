#include "Flappy.h"

namespace Flappy {
    struct Obstacle {
        int x;
        int topHeight;
        bool scored;
    };

    const int maxObstacles = 5;
    Obstacle* obstacles = nullptr;
    int numObstacles;
    int birdY; // top y position, 0 at top
    int birdVel;
    bool isAlive;
    int score;
    int speed;
    int nextObsCounter;
    const int birdX = 30;
    const int birdWidth = 8;
    const int birdHeight = 8;
    const int pipeWidth = 10;
    const int gapHeight = 28;

    // Bitmask for bird (8x8, 1 byte per row)
    const uint8_t birdBitmap[8] = {
        0b00000000,
        0b00111100,
        0b11110010,
        0b11110011,
        0b11111110,
        0b01111100,
        0b00000000,
        0b00000000
    }; // Simple bird shape

    void init() {
        Runtime::updatePeriod = 50;
        Runtime::showPeriod = 50;

        if (obstacles == nullptr) {
            obstacles = new Obstacle[maxObstacles];
        }
        numObstacles = 0;

        birdY = (SCREEN_H / 2) - (birdHeight / 2);
        birdVel = -5;
        isAlive = false;
        score = 0;
        speed = 4;
        nextObsCounter = 30 + rand() % 20; // initial delay in frames
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
            // Draw bird
            System::display.drawBitmap(birdX, birdY, birdBitmap, birdWidth, birdHeight, SH110X_WHITE);

            // Draw obstacles
            for (int i = 0; i < numObstacles; ++i) {
                // Top pipe
                System::display.fillRect(obstacles[i].x, 0, pipeWidth, obstacles[i].topHeight, SH110X_WHITE);
                // Bottom pipe
                int bottomY = obstacles[i].topHeight + gapHeight;
                int bottomH = SCREEN_H - bottomY;
                System::display.fillRect(obstacles[i].x, bottomY, pipeWidth, bottomH, SH110X_WHITE);
            }

            // Draw score
            System::display.setTextSize(1);
            System::display.setTextColor(SH110X_WHITE);
            System::display.setCursor(SCREEN_W - 30, 0);
            System::display.print(score);
        } else {
            if (score == 0) {
                drawCenteredText("Flappy", 15, 2, SH110X_WHITE);
                drawCenteredText("Press to start", 40, 1, SH110X_WHITE);
            } else {
                drawCenteredText("Game Over!", 15, 2, SH110X_WHITE);
                drawCenteredText("Score: " + String(score), 35, 1, SH110X_WHITE);
                drawCenteredText("Press to restart", 50, 1, SH110X_WHITE);
            }
        }

        System::display.display();
    }

    void update() {
        if (System::input.analogButtons[0].released) {
            Runtime::closeApp();
        }

        if (!isAlive) {
            if (System::input.joystickButton.released || System::input.joystick.lastUpdated == System::Direction::Up) {
                numObstacles = 0;
                birdY = (SCREEN_H / 2) - (birdHeight / 2);
                birdVel = -5;
                isAlive = true;
                score = 0;
                speed = 4;
                nextObsCounter = 30 + rand() % 20;
            }
            return;
        }

        // Handle flap
        if (System::input.joystick.lastUpdated == System::Direction::Up || System::input.analogButtons[1].released) {
            birdVel = -5; // flap upward
        }

        // Update bird physics
        birdY += birdVel;
        birdVel += 1; // gravity downward

        // Check bounds
        if (birdY < 0 || birdY + birdHeight > SCREEN_H) {
            isAlive = false;
        }

        // Update speed over time
        speed = 4 + score / 10;

        // Move obstacles
        for (int i = 0; i < numObstacles; ++i) {
            obstacles[i].x -= speed;
        }

        // Remove off-screen obstacles
        while (numObstacles > 0 && obstacles[0].x < -pipeWidth) {
            for (int i = 1; i < numObstacles; ++i) {
                obstacles[i-1] = obstacles[i];
            }
            --numObstacles;
        }

        // Generate new obstacle
        --nextObsCounter;
        if (nextObsCounter <= 0 && numObstacles < maxObstacles) {
            Obstacle obs;
            obs.x = SCREEN_W;
            obs.topHeight = 10 + rand() % (SCREEN_H - gapHeight - 20);
            obs.scored = false;
            obstacles[numObstacles++] = obs;
            nextObsCounter = 30 + rand() % 20; // random frames between obstacles
        }

        // Check collisions and scores
        int birdRight = birdX + birdWidth;
        int birdBottom = birdY + birdHeight;
        for (int i = 0; i < numObstacles; ++i) {
            int obsLeft = obstacles[i].x;
            int obsRight = obstacles[i].x + pipeWidth;
            // Top pipe
            int topBottom = obstacles[i].topHeight;
            if (birdRight > obsLeft && birdX < obsRight &&
                birdBottom > 0 && birdY < topBottom) {
                isAlive = false;
                break;
            }
            // Bottom pipe
            int bottomY = obstacles[i].topHeight + gapHeight;
            int bottomBottom = SCREEN_H;
            if (birdRight > obsLeft && birdX < obsRight &&
                birdBottom > bottomY && birdY < bottomBottom) {
                isAlive = false;
                break;
            }
            // Score if passed
            if (!obstacles[i].scored && obstacles[i].x + pipeWidth < birdX) {
                obstacles[i].scored = true;
                score++;
            }
        }

        Runtime::wasUpdate = true;
    }

    void clean() {
        if (obstacles != nullptr) {
            delete[] obstacles;
            obstacles = nullptr;
        }
    }
}