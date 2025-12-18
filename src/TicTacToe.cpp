#include "TicTacToe.h"

namespace TicTacToe {
    char* board = nullptr; // flattened 3x3 array: ' ', 'X', 'O'
    char currentPlayer;
    int cursorX, cursorY; // 0-2
    bool gameOver;
    char winner; // ' ' none, 'X'/'O' winner, 'D' draw

    const int offsetX = 4;
    const int offsetY = 2;
    const int cellWidth = 40;
    const int cellHeight = 20;

    void init() {
        Runtime::updatePeriod = 100;
        Runtime::showPeriod = 100;

        if (board == nullptr) {
            board = new char[9];
        }

        for (int i = 0; i < 9; ++i) {
            board[i] = ' ';
        }

        currentPlayer = 'X';
        cursorX = 1;
        cursorY = 1;
        gameOver = false;
        winner = ' ';
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

    bool checkWin(char player) {
        // Rows
        for (int i = 0; i < 3; ++i) {
            if (board[i*3] == player && board[i*3+1] == player && board[i*3+2] == player) return true;
        }
        // Columns
        for (int i = 0; i < 3; ++i) {
            if (board[i] == player && board[i+3] == player && board[i+6] == player) return true;
        }
        // Diagonals
        if (board[0] == player && board[4] == player && board[8] == player) return true;
        if (board[2] == player && board[4] == player && board[6] == player) return true;
        return false;
    }

    bool checkDraw() {
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') return false;
        }
        return true;
    }

    void drawX(int cx, int cy) {
        System::display.drawLine(cx + 5, cy + 5, cx + 35, cy + 15, SH110X_WHITE);
        System::display.drawLine(cx + 5, cy + 15, cx + 35, cy + 5, SH110X_WHITE);
    }

    void drawO(int cx, int cy) {
        System::display.drawCircle(cx + 20, cy + 10, 10, SH110X_WHITE);
    }

    void show() {
        System::display.clearDisplay();

        if (!gameOver) {
            // Draw grid lines
            // Vertical lines
            System::display.drawLine(offsetX + cellWidth, offsetY, offsetX + cellWidth, offsetY + 3 * cellHeight, SH110X_WHITE);
            System::display.drawLine(offsetX + 2 * cellWidth, offsetY, offsetX + 2 * cellWidth, offsetY + 3 * cellHeight, SH110X_WHITE);
            // Horizontal lines
            System::display.drawLine(offsetX, offsetY + cellHeight, offsetX + 3 * cellWidth, offsetY + cellHeight, SH110X_WHITE);
            System::display.drawLine(offsetX, offsetY + 2 * cellHeight, offsetX + 3 * cellWidth, offsetY + 2 * cellHeight, SH110X_WHITE);

            // Draw board content
            for (int y = 0; y < 3; ++y) {
                for (int x = 0; x < 3; ++x) {
                    int cx = offsetX + x * cellWidth;
                    int cy = offsetY + y * cellHeight;
                    char cell = board[y*3 + x];
                    if (cell == 'X') {
                        drawX(cx, cy);
                    } else if (cell == 'O') {
                        drawO(cx, cy);
                    }
                }
            }

            // Highlight cursor with a rounded rectangle
            int cx = offsetX + cursorX * cellWidth;
            int cy = offsetY + cursorY * cellHeight;
            System::display.drawRoundRect(cx - 1, cy - 1, cellWidth + 2, cellHeight + 2, 3, SH110X_WHITE);
        } else {
            String msg = (winner == 'D') ? "Draw!" : String(winner) + " wins!";
            drawCenteredText(msg, 20, 2, SH110X_WHITE);
            drawCenteredText("Press to restart", 50, 1, SH110X_WHITE);
        }

        System::display.display();
    }

    void update() {
        if (System::input.analogButtons[0].held) {
            Runtime::closeApp();
        }

        if (gameOver) {
            if (System::input.joystickButton.held) {
                init();
            }
            return;
        }

        // Handle movement
        if (System::input.joystick.up.entered || System::input.analogButtons[1].held) {
            if (cursorY > 0) --cursorY;
        } else if (System::input.joystick.down.entered || System::input.analogButtons[2].held) {
            if (cursorY < 2) ++cursorY;
        } else if (System::input.joystick.left.entered || System::input.analogButtons[3].held) {
            if (cursorX > 0) --cursorX;
        } else if (System::input.joystick.right.entered || System::input.analogButtons[4].held) {
            if (cursorX < 2) ++cursorX;
        }

        // Place mark
        if (System::input.joystickButton.held) {
            int pos = cursorY * 3 + cursorX;
            if (board[pos] == ' ') {
                board[pos] = currentPlayer;
                if (checkWin(currentPlayer)) {
                    gameOver = true;
                    winner = currentPlayer;
                } else if (checkDraw()) {
                    gameOver = true;
                    winner = 'D';
                } else {
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                }
            }
        }

        Runtime::wasUpdate = true;
    }

    void clean() {
        if (board != nullptr) {
            delete[] board;
            board = nullptr;
        }
    }
}