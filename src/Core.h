#ifndef CORE_H
#define CORE_H

#include "Arduino.h"
#include "Adafruit_SH110X.h"

#define SCREEN_H 64
#define SCREEN_W 128

constexpr int JOYSTICK_BUTTON_PIN = 10;
constexpr int JOYSTICK_X_PIN = A1;
constexpr int JOYSTICK_Y_PIN = A0;
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

    enum class Axis { X, Y };
    enum class CrossDirection { Positive, Negative };
    enum class Direction { Center, Up, Down, Left, Right };

    class DigitalButton {
    private:
        int pin; 
        bool currentState = false; 
        bool lastReading = false; 
        unsigned long lastDebounceTime = 0; 
        const unsigned long debounceDelay;

        bool _isPressed = false;
        bool _isReleased = false;
    public:
        DigitalButton(int p, unsigned long delay = 50);

        void init();

        void read();

        bool isPressed();
        bool isReleased();

        bool isHeld() const;
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

    struct ButtonState {
        bool pressed = false;    // Was pressed this frame
        bool released = false;   // Was released this frame
        bool held = false;       // Is currently held
    };

    struct DirectionState {
        bool entered = false;    // Entered direction this frame
        bool exited = false;     // Exited direction this frame
        bool held = false;       // Is currently in this direction
    };

    struct AxisThresholdState {
        int threshold;           // The threshold value (positive or negative)
        bool crossedPositive = false;  // Crossed into positive zone
        bool crossedNegative = false;  // Crossed into negative zone
        bool uncrossedPositive = false; // Exited positive zone
        bool uncrossedNegative = false; // Exited negative zone
    };

    struct JoystickState {
        int x = 512;
        int y = 512;
        bool moved = false;      // Outside deadzone this frame

        DirectionState up;
        DirectionState down;
        DirectionState left;
        DirectionState right;

        // Custom thresholds: array of states, matching customThresholds
        AxisThresholdState* customX = nullptr;  // Dynamically allocated if needed, or fixed
        AxisThresholdState* customY = nullptr;
        uint8_t customXCount = 0;
        uint8_t customYCount = 0;
    };

    // Global input state
    struct InputState {
        ButtonState joystickButton;
        ButtonState analogButtons[5];  // Fixed for your 5 buttons

        JoystickState joystick;

        // Function to check custom threshold (example usage)
        bool wasCrossed(Axis axis, int thresh, CrossDirection dir) const;
    };

    extern InputState input;  // Global, updated in handleInput()

    // Customizable thresholds (as before)
    struct AxisThreshold {
        Axis axis;
        int value;  // Absolute positive value
    };
    constexpr AxisThreshold customThresholds[] = {
        {Axis::X, 200},
        {Axis::X, 400},
        {Axis::Y, 300}
    };
    constexpr uint8_t THRESHOLD_COUNT = sizeof(customThresholds) / sizeof(AxisThreshold);
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