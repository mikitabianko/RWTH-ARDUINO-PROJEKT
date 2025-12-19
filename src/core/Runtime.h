#ifndef RUNTIME_H
#define RUNTIME_H

#include "System.h"
#include "App.h"

namespace Runtime {
    extern bool wasUpdate;
    extern unsigned long updatePeriod;
    extern unsigned long showPeriod;
    
    extern App::App app;

    void setApp(App::App newApp);

    void closeApp();

    void setDefaultApp(App::App newApp);

    void tick();
}
#endif