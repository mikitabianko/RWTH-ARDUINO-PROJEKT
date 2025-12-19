#include "Runtime.h"

namespace Runtime {
    bool wasUpdate = true;
    unsigned long updatePeriod = 100;
    unsigned long showPeriod = 100;
    App::App app = App::defaultApp;

    unsigned long t1, t2, now;

    void tick() {
        now = millis();
        System::handleInput();
        
        if (now - t1 >= updatePeriod) {
            app.update();
            System::eventsClear();
            t1 = now;
        }
        if (now - t2 >= showPeriod) {
            if (wasUpdate) app.show();
            wasUpdate = false;
            t2 = now;
        }
    }

    void setApp(App::App newApp) {
        // app.clean();
        app = newApp;
        app.init();
        System::eventsClear();
    }

    void setDefaultApp(App::App newApp) {
        App::defaultApp = newApp;
    }

    void closeApp() {
        app = App::defaultApp;
        app.init();
        wasUpdate = true;
    }
}