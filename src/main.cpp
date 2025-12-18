#include "Core.h" // Funduino

#include "Menu.h"
#include "Game1.h"
#include "Snake.h"
#include "SlotMachine.h"
#include "TicTacToe.h"
#include "Dino.h"
#include "Flappy.h"

namespace Menu {
    App::App games[] = {
        {
            "game1", Game1::precalc, Game1::render, Game1::update, Game1::clean
        },
        {
            "Snake", Snake::init, Snake::show, Snake::update, Snake::clean
        },
        {
            "Casino", SlotMachine::init, SlotMachine::show, SlotMachine::update, SlotMachine::clean
        },
        {
            "TicTacToe", TicTacToe::init, TicTacToe::show, TicTacToe::update, TicTacToe::clean
        },
        {
            "Dino", Dino::init, Dino::show, Dino::update, Dino::clean
        },
        {
            "Flappy", Flappy::init, Flappy::show, Flappy::update, Flappy::clean
        }
    };
    int gamesCount = sizeof(games) / sizeof(App::App);
}

void setup() {
    System::setup();

#ifdef DEBUG
    Serial.print(Menu::gamesCount);
    Serial.println(" games added");
#endif
    Runtime::setDefaultApp(Menu::menu);
    Runtime::setApp(Menu::menu);
}

void loop() {
    Runtime::tick();
}