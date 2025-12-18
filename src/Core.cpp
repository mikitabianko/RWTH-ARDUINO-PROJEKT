#include "Core.h"

namespace System {
    Adafruit_SH1106G display(SCREEN_W, SCREEN_H, &Wire, -1);

    DigitalButton joystickButton(JOYSTICK_BUTTON_PIN, 50);
    Joystick joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);
    AnalogButtons analogButtons(BUTTONS_PIN, analogThresholds, 5, 50);

    DigitalButton::DigitalButton(int p, unsigned long delay) : pin(p), debounceDelay(delay) {}

    InputState input;

    Direction getCurrentDirection(int dx, int dy, int threshold = 150) {
        if (abs(dx) < threshold && abs(dy) < threshold) return Direction::Center;

        if (abs(dx) > abs(dy)) {
            return (dx > 0) ? Direction::Right : Direction::Left;
        } 
        return (dy > 0) ? Direction::Down : Direction::Up;  
    }

    void DigitalButton::init() {
        pinMode(pin, INPUT_PULLUP);
        lastReading = digitalRead(pin) == LOW;
        currentState = lastReading;
    }

    void DigitalButton::read() {
        bool reading = digitalRead(pin) == LOW; 
        _isReleased = false;
        _isPressed = false;

        if (reading != lastReading) {
            lastDebounceTime = millis(); 
            lastReading = reading;
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading != currentState) {
                bool previousState = currentState;
                currentState = reading;

                if (currentState && !previousState) {
                    _isPressed = true;
                } else if (!currentState && previousState) {
                    _isReleased = true;
                }
            }
        }
    }

    bool DigitalButton::isPressed() {
        return _isPressed;
    }

    bool DigitalButton::isReleased() {
        return _isReleased;
    }

    bool DigitalButton::isHeld() const {
        return currentState;
    }

    AnalogButtons::AnalogButtons(int p, const ButtonThreshold* thresh, uint8_t count, unsigned long delay)
        : pin(p), thresholds(const_cast<ButtonThreshold*>(thresh)), buttonCount(count), debounceDelay(delay) {
        
        currentState     = 0;
        lastReading      = 0;
        _isPressed       = 0;
        _isReleased      = 0;
    }

    AnalogButtons::~AnalogButtons() {}

    void AnalogButtons::init() {
        AnalogButtons::read();
    }

    void AnalogButtons::read() {
        if ((millis() - lastDebounceTime) < debounceDelay) return;

        int analogValue = analogRead(pin);

        if (abs(lastAnalogValue - analogValue) < 50) return;
        lastAnalogValue = analogValue;  

        int activeButton = -1;  
        for (uint8_t i = 0; i < buttonCount; i++) {
            if (analogValue >= thresholds[i].min && analogValue <= thresholds[i].max) {
                activeButton = i;
                break;  
            }
        }

        byte newState = 0;
        if (activeButton >= 0) {
            newState = (1 << activeButton);
        }

        if (newState != currentState) {
            byte previousState = currentState;
            currentState = newState;

            _isPressed = currentState & ~previousState;   
            _isReleased = previousState & ~currentState; 
        }

        if (newState != currentState) {
            lastDebounceTime = millis();
        }
    }

    bool AnalogButtons::isPressed(uint8_t index) const {
        if (index >= buttonCount) return false;
        return (_isPressed >> index) & 1;
    }

    bool AnalogButtons::isReleased(uint8_t index) const {
        if (index >= buttonCount) return false;
        return (_isReleased >> index) & 1;
    }

    bool AnalogButtons::isHeld(uint8_t index) const {
        if (index >= buttonCount) return false;
        return (currentState >> index) & 1;
    }

    int AnalogButtons::getRawValue() const {
        return lastAnalogValue;
    }

    uint8_t AnalogButtons::getButtonCount() const {
        return buttonCount;
    }
    
    void Joystick::init() {}

    void Joystick::read() { // to do "isInversed"!
        xValue = analogRead(xPin);
        yValue = 1023 - analogRead(yPin);
    }

    bool Joystick::isMoved() const {
        return (abs(xValue - 512) > deadZone || abs(yValue - 512) > deadZone);
    }

    void setup() {
#ifdef DEBUG
        Serial.begin(9600);
#endif

        unsigned long startTime = millis();
        bool displayStarted = false;
        while (!displayStarted && (millis() - startTime < 5000)) {
            displayStarted = display.begin(0x3C);
            if (!displayStarted) delay(100);  
        }

        if (!displayStarted) {
#ifdef DEBUG
            Serial.println("Display init failed after timeout!");
#endif
            while(1); 
        } else {
#ifdef DEBUG
            Serial.println("Display was connected");
            Serial.println("Display is cleared");
#endif
            display.clearDisplay();
            display.display();
        }

        joystickButton.init();
        joystick.init();
        analogButtons.init();

#ifdef DEBUG
        Serial.println("Input devices are initialized");
#endif
        srand(analogRead(A3));
        uint8_t xCount = 0, yCount = 0;
        for (uint8_t i = 0; i < THRESHOLD_COUNT; ++i) {
            if (customThresholds[i].axis == Axis::X) xCount++;
            else yCount++;
        }
        input.joystick.customXCount = xCount;
        input.joystick.customYCount = yCount;
        if (xCount > 0) input.joystick.customX = new AxisThresholdState[xCount];
        if (yCount > 0) input.joystick.customY = new AxisThresholdState[yCount];

        // Initialize thresholds
        uint8_t xIdx = 0, yIdx = 0;
        for (uint8_t i = 0; i < THRESHOLD_COUNT; ++i) {
            const auto& t = customThresholds[i];
            if (t.axis == Axis::X) {
                input.joystick.customX[xIdx++].threshold = t.value;
            } else {
                input.joystick.customY[yIdx++].threshold = t.value;
            }
        }
    }

    // Helper to get the right AxisThresholdState (assuming customX/Y are arrays)
    AxisThresholdState& getThresholdState(Axis axis, uint8_t idx) {
        return (axis == Axis::X) ? input.joystick.customX[idx] : input.joystick.customY[idx];
    }

    void eventsClear() {
        input.joystickButton.pressed = false;
        input.joystickButton.released = false;
        input.joystickButton.held = false;

        for (uint8_t i = 0; i < 5; ++i) {
            input.analogButtons[i].pressed = false;
            input.analogButtons[i].released = false;
            input.analogButtons[i].held = false;
        }

        input.joystick.moved = false;
        input.joystick.up.entered = false;
        input.joystick.up.exited = false;
        input.joystick.up.held = false;
        input.joystick.down.entered = false;
        input.joystick.down.exited = false;
        input.joystick.down.held = false;
        input.joystick.left.entered = false;
        input.joystick.left.exited = false;
        input.joystick.left.held = false;
        input.joystick.right.entered = false;
        input.joystick.right.exited = false;
        input.joystick.right.held = false;

        for (uint8_t i = 0; i < input.joystick.customXCount; ++i) {
            input.joystick.customX[i].crossedPositive = false;
            input.joystick.customX[i].uncrossedPositive = false;
            input.joystick.customX[i].crossedNegative = false;
            input.joystick.customX[i].uncrossedNegative = false;
        }
        for (uint8_t i = 0; i < input.joystick.customYCount; ++i) {
            input.joystick.customY[i].crossedPositive = false;
            input.joystick.customY[i].uncrossedPositive = false;
            input.joystick.customY[i].crossedNegative = false;
            input.joystick.customY[i].uncrossedNegative = false;
        }
    }

    void handleInput() {
        joystick.read();
        joystickButton.read();
        analogButtons.read();

        // Poll
        joystickButton.read();
        analogButtons.read();
        joystick.read();

        // Joystick button
        input.joystickButton.pressed |= joystickButton.isPressed();
        input.joystickButton.released |= joystickButton.isReleased();
        input.joystickButton.held |= joystickButton.isHeld();

        // Analog buttons
        for (uint8_t i = 0; i < analogButtons.getButtonCount(); ++i) {
            input.analogButtons[i].pressed |= analogButtons.isPressed(i);
            input.analogButtons[i].released |= analogButtons.isReleased(i);
            input.analogButtons[i].held |= analogButtons.isHeld(i);
        }

        // Joystick
        static int prevX = 512;
        static int prevY = 512;
        static Direction prevDir = Direction::Center;

        int currX = joystick.getX();
        int currY = joystick.getY();
        input.joystick.x = currX;
        input.joystick.y = currY;
        input.joystick.moved = joystick.isMoved();

        int dx = currX - 512;
        int dy = currY - 512;
        Direction currDir = getCurrentDirection(dx, dy);

        // Direction
        if (currDir != prevDir) {
            if (prevDir != Direction::Center) {
                DirectionState& ds = (prevDir == Direction::Up) ? input.joystick.up :
                                    (prevDir == Direction::Down) ? input.joystick.down :
                                    (prevDir == Direction::Left) ? input.joystick.left : input.joystick.right;
                ds.exited = true;
            }
            if (currDir != Direction::Center) {
                DirectionState& ds = (currDir == Direction::Up) ? input.joystick.up :
                                    (currDir == Direction::Down) ? input.joystick.down :
                                    (currDir == Direction::Left) ? input.joystick.left : input.joystick.right;
                ds.entered = true;
                ds.held = true;
            }
        } else if (currDir != Direction::Center) {
            DirectionState& ds = (currDir == Direction::Up) ? input.joystick.up :
                                (currDir == Direction::Down) ? input.joystick.down :
                                (currDir == Direction::Left) ? input.joystick.left : input.joystick.right;
            ds.held = true;
        }

        // Custom thresholds
        uint8_t xIdx = 0;
        uint8_t yIdx = 0;
        for (uint8_t i = 0; i < THRESHOLD_COUNT; ++i) {
            const auto& thresh = customThresholds[i];
            int prevVal = (thresh.axis == Axis::X) ? prevX : prevY;
            int currVal = (thresh.axis == Axis::X) ? currX : currY;
            int t = thresh.value;

            if (thresh.axis == Axis::X && xIdx < input.joystick.customXCount) {
                // Positive
                bool prevPos = prevVal >= t;
                bool currPos = currVal >= t;
                if (!prevPos && currPos) {
                    input.joystick.customX[xIdx].crossedPositive = true;
                } else if (prevPos && !currPos) {
                    input.joystick.customX[xIdx].uncrossedPositive = true;
                }

                // Negative
                bool prevNeg = prevVal <= -t;
                bool currNeg = currVal <= -t;
                if (!prevNeg && currNeg) {
                    input.joystick.customX[xIdx].crossedNegative = true;
                } else if (prevNeg && !currNeg) {
                    input.joystick.customX[xIdx].uncrossedNegative = true;
                }
                xIdx++;
            } else if (thresh.axis == Axis::Y && yIdx < input.joystick.customYCount) {
                // Positive
                bool prevPos = prevVal >= t;
                bool currPos = currVal >= t;
                if (!prevPos && currPos) {
                    input.joystick.customY[yIdx].crossedPositive = true;
                } else if (prevPos && !currPos) {
                    input.joystick.customY[yIdx].uncrossedPositive = true;
                }

                // Negative
                bool prevNeg = prevVal <= -t;
                bool currNeg = currVal <= -t;
                if (!prevNeg && currNeg) {
                    input.joystick.customY[yIdx].crossedNegative = true;
                } else if (prevNeg && !currNeg) {
                    input.joystick.customY[yIdx].uncrossedNegative = true;
                }
                yIdx++;
            }
        }

        // Update prev
        prevX = currX;
        prevY = currY;
        prevDir = currDir;

#ifdef DEBUG
        bool info = false;
        static bool wasMoved = false;
        bool nowMoved = joystick.isMoved();


        info |= nowMoved != wasMoved || nowMoved;
        info |= joystickButton.isPressed();
        info |= joystickButton.isReleased();
        info |= joystickButton.isHeld();
        for (int i = 0; i < 5; ++i) 
            info |= analogButtons.isPressed(i);

        wasMoved = nowMoved;

        if (!info) return;
        Serial.print("J: X=");
        Serial.print(joystick.getX());
        Serial.print(", Y=");
        Serial.print(joystick.getY());

        if (nowMoved) {
            Serial.print(" m ");
        } else {
            Serial.print(" c ");
        }

        Serial.print("JB: ");

        if (joystickButton.isPressed()) {
            Serial.print("P ");
        } else if (joystickButton.isReleased()) {
            Serial.print("R ");
        } else if (joystickButton.isHeld()) {
            Serial.print("H ");
        } else {
            Serial.print("N ");
        }
        Serial.print("A:");
        for (int i = 0; i < 5; ++i) 
            Serial.print(analogButtons.isPressed(i));

        Serial.print(input.joystick.left.held);
        Serial.print(input.joystick.left.entered);
        Serial.print(input.joystick.left.exited);
        Serial.print(input.joystickButton.released);
        Serial.println();
#endif
    }


}

namespace InitApp {
    void init() {}
    void update() {}
    void show() {}
    void clear() {}
}

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

namespace App {
    App defaultApp("initApp", InitApp::init, InitApp::update, InitApp::show, InitApp::clear);
}