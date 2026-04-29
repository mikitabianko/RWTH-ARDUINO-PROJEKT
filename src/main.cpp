#include "core/System.h" 
#include "core/Runtime.h"

#include "Config.h"

#include "drivers/display/SH110X/SH110X_Driver.h"
#include "drivers/digitalButton/DigitalButton_Driver.h"

#include "apps/Menu/Menu.h"

#include "games/Game1/Game1.h"
#include "games/Snake/Snake.h"
#include "games/SlotMachine/SlotMachine.h"
#include "games/TicTacToe/TicTacToe.h"
#include "games/Dino/Dino.h"
#include "games/Flappy/Flappy.h"

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

static Drivers::SH110X_Driver Display(
    Config::Display::Width, 
    Config::Display::Height, 
    Config::Display::Address
);

static Drivers::DigitalButton_Driver physicalJoyBtn(Config::Pins::JoystickBtn);

void setup() {
    System::setDisplay(&Display);
    System::setJoyButton(&physicalJoyBtn);
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