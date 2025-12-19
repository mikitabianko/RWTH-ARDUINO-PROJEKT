#include "App.h"

namespace InitApp {
    void init() {}
    void update() {}
    void show() {}
    void clear() {}
}

namespace App {
    App defaultApp("initApp", InitApp::init, InitApp::update, InitApp::show, InitApp::clear);
}