#ifndef CORE_H
#define CORE_H

#include "Arduino.h"
#include "Adafruit_SH110X.h"

#define SCREEN_H 64
#define SCREEN_W 128

constexpr int JOYSTICK_BUTTON_PIN = 10;
constexpr int JOYSTICK_X_PIN = A0;
constexpr int JOYSTICK_Y_PIN = A1;
constexpr int BUTTONS_PIN = A2;

enum class ButtonId {
  None = 0,
  Button1,
  Button2,
  Button3,
  Button4,
  Button5
};

namespace System {
    extern Adafruit_SH1106G display;

    class DigitalButton {
    private:
        int pin; 
        bool currentState = false; 
        bool lastReading = false; 
        unsigned long lastDebounceTime = 0; 
        const unsigned long debounceDelay;

        bool _isPressed = false;
        bool _isReleased = false;
        bool _isPressedLatch = false;   
        bool _isReleasedLatch = false;
    public:
        DigitalButton(int p, unsigned long delay = 50);

        void init();

        void read();

        bool isPressed();
        bool isReleased();

        bool isHeld() const;

        bool consumePressed();

        bool consumeReleased();

        bool consumePressedEvent();
        bool consumeReleasedEvent();
    };

    struct ButtonThreshold {
        int min;
        int max;
    };

    class AnalogButtons {
    private:
        int pin;
        const ButtonThreshold* thresholds;
        uint8_t buttonCount;
        const unsigned long debounceDelay;

        byte currentState;
        byte lastReading;
        byte _isPressed; 
        byte _isReleased;
        byte _isPressedLatch;
        byte _isReleasedLatch;

        unsigned long lastDebounceTime = 0;
        int lastAnalogValue = 0;

    public:
        AnalogButtons(int p, const ButtonThreshold* thresh, uint8_t count, unsigned long delay = 50);
        ~AnalogButtons();

        void init();
        void read();

        bool isPressed(uint8_t index) const;
        bool isReleased(uint8_t index) const;
        bool isHeld(uint8_t index) const;

        int getRawValue() const;
        uint8_t getButtonCount() const;

        bool consumePressed(uint8_t index);

        bool consumeReleased(uint8_t index);

        bool consumePressedEvent(uint8_t index);
        bool consumeReleasedEvent(uint8_t index);
    };

    class Joystick {
    private:
        int xPin;
        int yPin;
        int xValue = 512; 
        int yValue = 512;
        int deadZone = 50;

    public:
        Joystick(int xp, int yp) : xPin(xp), yPin(yp) {}
        void init();
        void read();
        int getX() const { return xValue; }
        int getY() const { return yValue; }
        bool isMoved() const;
    };

    constexpr ButtonThreshold analogThresholds[5] = {
        {150, 250},    
        {350, 450},
        {550, 650},
        {750, 900},
        {950, 1024}
    };

    extern DigitalButton joystickButton;
    extern Joystick joystick;
    extern AnalogButtons analogButtons;
    
    void setup();

    void handleInput();
}

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