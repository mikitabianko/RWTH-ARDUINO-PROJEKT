#include <Arduino.h>
#include "Core.h"

#include "menu.h"
#include "game1.h"
#include "snake.h"

namespace Menu {
    App::App games[] = {
        {
            "game1", Game1::precalc, Game1::render, Game1::update, Game1::clean
        },
        {
            "game2", Game1::precalc, Game1::render, Game1::update, Game1::clean
        },
        {
            "game3", Game1::precalc, Game1::render, Game1::update, Game1::clean
        },
        {
            "game4", Game1::precalc, Game1::render, Game1::update, Game1::clean
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