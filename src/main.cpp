#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include "menu.h"
#include "game1.h"

#define SCREEN_H 64
#define SCREEN_W 128
Adafruit_SH1106G display(SCREEN_W, SCREEN_H, &Wire, -1);

bool wasUpdate = true;

const int joystickButtonPin = 10;
const int joystickXPin = A1;
const int joystickYPin = A0;

unsigned long drawTimer;  
unsigned long inputProcessTimer;  
unsigned long joystickTimer;
unsigned long buttonTimer;
unsigned long now;

int joystickX, joystickY;
int joystickButtonState;

void (*draw)(Adafruit_SH1106G& display, int screen_h, int screen_w);
void (*update)(int joystickX, int joystickY, int joystickButtonState);
// void (*gameState)();

const int gamesCount = 1;
Game games[gamesCount] = {
    {
        "game1", Game1::precalc, Game1::render, Game1::update
    }
};

void setup() {
    // Serial.begin(9600);
    pinMode(joystickButtonPin, INPUT_PULLUP);

    while (!display.begin(0x3C));
    display.clearDisplay();
    display.display();

    draw = drawMenu;
    update = processMenu;
}

void loop() {
    now = millis();  
    if (now - joystickTimer >= 10) {
        joystickX = analogRead(joystickXPin);
        joystickY = analogRead(joystickYPin);
        joystickTimer = now;
    }

    if (now - buttonTimer >= 150) {
        joystickButtonState = digitalRead(joystickButtonPin);
        buttonTimer = now;
    }

    if (now - drawTimer >= 50)  {
        if (wasUpdate) {
            draw(display, SCREEN_H, SCREEN_W);
            wasUpdate = false;
        }
        drawTimer = now;
    }

    if (now - inputProcessTimer >= 200) {
        update(joystickX, joystickY, joystickButtonState);
        if (gameToRun != -1) {
            games[gameToRun].init();
            draw = games[gameToRun].draw;
            update = games[gameToRun].update;
            gameToRun = -1;
        } 
        wasUpdate = true;
        inputProcessTimer = now;
    }
}