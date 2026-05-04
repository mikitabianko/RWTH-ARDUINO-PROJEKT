#ifndef SYSTEM_H
#define SYSTEM_H

#include "Arduino.h"
#include "drivers/common/IDisplay.h"
#include "drivers/common/IDigitalButton.h"
#include "drivers/common/IAnalogButtons.h"

#define SCREEN_H 64
#define SCREEN_W 128

constexpr int JOYSTICK_X_PIN = A1;
constexpr int JOYSTICK_Y_PIN = A0;

namespace System {
    extern Drivers::IDisplay* display;
    extern Drivers::IDigitalButton* joystickButton;
    extern Drivers::IAnalogButtons* analogButtons;

    void setJoyButton(Drivers::IDigitalButton* driver);
    void setDisplay(Drivers::IDisplay* driver);
    void setAnalogButtons(Drivers::IAnalogButtons* driver);

    enum class Axis { X, Y };
    enum class CrossDirection { Positive, Negative };
    enum class Direction { Center, Up, Down, Left, Right };

    class Joystick {
    private:
        int xPin;
        int yPin;
        int xValue = 512; 
        int yValue = 512;

    public:
        const int deadZone;

        Joystick(int xp, int yp, int deadZone = 100) : xPin(xp), yPin(yp), deadZone(deadZone) {}
        void init();
        void read();
        int getX() const { return xValue; }
        int getY() const { return yValue; }
        bool isMoved() const;
    };

    // constexpr ButtonThreshold analogThresholds[5] = {
    //     {150, 250},    
    //     {350, 450},
    //     {550, 650},
    //     {750, 900},
    //     {950, 1024}
    // };

    extern Joystick joystick;
    
    void setDisplay(Drivers::IDisplay* driver);

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

        Direction lastUpdated;

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

    void eventsClear();
}
#endif