#ifndef APP_H
#define APP_H
#include "Arduino.h"

namespace App {
    class App {
    public:
        String Name;
        void (*init)();
        void (*show)();
        void (*update)();
        void (*clean)();

        App(String name, 
            void (*init)(),
            void (*show)(),
            void (*update)(),
            void (*clean)()) 
        : Name(name), init(init), show(show), update(update), clean(clean) {}

        App();
    };

    extern App defaultApp;
}
#endif