#ifndef MENU_H
#define MENU_H

#include "core/System.h"
#include "core/Runtime.h"

namespace Menu {
    void init();
    void drawMenu();
    void processMenu();
    void clear();

    void returnToMenu();

    extern int gamesCount;
    extern App::App games[];

    extern App::App menu;
}
#endif