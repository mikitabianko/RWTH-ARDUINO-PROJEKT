#include "System.h"

namespace System {
    Drivers::IDisplay* display = nullptr;
    Drivers::IDigitalButton* joystickButton = nullptr;
    Drivers::IAnalogButtons* analogButtons = nullptr;

    void setDisplay(Drivers::IDisplay* driver) {
        display = driver;
    }

    void setJoyButton(Drivers::IDigitalButton* driver) {
        joystickButton = driver;
    }

    void setAnalogButtons(Drivers::IAnalogButtons* driver) {
        analogButtons = driver;
    }

    Joystick joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);

    InputState input;

    Direction getCurrentDirection(int dx, int dy, int threshold = 150) {
        if (abs(dx) < threshold && abs(dy) < threshold) return Direction::Center;

        if (abs(dx) > abs(dy)) {
            return (dx > 0) ? Direction::Right : Direction::Left;
        } 
        return (dy > 0) ? Direction::Down : Direction::Up;  
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
            displayStarted = display->begin();
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
            display->clear();
            display->display();
        }

        joystickButton->init();
        joystick.init();
        analogButtons->init();

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

        input.joystick.lastUpdated = Direction::Center;

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
        joystickButton->read();
        analogButtons->read();

        input.joystickButton.pressed |= joystickButton->isPressed();
        input.joystickButton.released |= joystickButton->isReleased();
        input.joystickButton.held |= joystickButton->isHeld();

        for (uint8_t i = 0; i < analogButtons->getButtonCount(); ++i) {
            input.analogButtons[i].pressed |= analogButtons->isPressed(i);
            input.analogButtons[i].released |= analogButtons->isReleased(i);
            input.analogButtons[i].held |= analogButtons->isHeld(i);
        }

        static int prevX = 512;
        static int prevY = 512;
        static Direction prevDir = Direction::Center;

        int currX = joystick.getX();
        int currY = joystick.getY();
        input.joystick.x = currX;
        input.joystick.y = currY;
        input.joystick.moved |= joystick.isMoved();

        int dx = currX - 512;
        int dy = currY - 512;
        Direction currDir = getCurrentDirection(dx, dy, joystick.deadZone);
        if (currDir != prevDir) {
            if (prevDir != Direction::Center) {
                DirectionState& ds = (prevDir == Direction::Up) ? input.joystick.up :
                                    (prevDir == Direction::Down) ? input.joystick.down :
                                    (prevDir == Direction::Left) ? input.joystick.left : input.joystick.right;
                ds.exited |= true;
            }
            if (currDir != Direction::Center) {
                DirectionState& ds = (currDir == Direction::Up) ? input.joystick.up :
                                    (currDir == Direction::Down) ? input.joystick.down :
                                    (currDir == Direction::Left) ? input.joystick.left : input.joystick.right;
                ds.entered |= true;
                ds.held |= true;
                input.joystick.lastUpdated = (currDir == Direction::Up) ? Direction::Up :
                                    (currDir == Direction::Down) ? Direction::Down :
                                    (currDir == Direction::Left) ? Direction::Left : Direction::Right;
            }
        } else if (currDir != Direction::Center) {
            DirectionState& ds = (currDir == Direction::Up) ? input.joystick.up :
                                (currDir == Direction::Down) ? input.joystick.down :
                                (currDir == Direction::Left) ? input.joystick.left : input.joystick.right;
            ds.held |= true;
        }

        uint8_t xIdx = 0;
        uint8_t yIdx = 0;
        for (uint8_t i = 0; i < THRESHOLD_COUNT; ++i) {
            const auto& thresh = customThresholds[i];
            int prevVal = (thresh.axis == Axis::X) ? prevX : prevY;
            int currVal = (thresh.axis == Axis::X) ? currX : currY;
            int t = thresh.value;

            if (thresh.axis == Axis::X && xIdx < input.joystick.customXCount) {
                bool prevPos = prevVal >= t;
                bool currPos = currVal >= t;
                if (!prevPos && currPos) {
                    input.joystick.customX[xIdx].crossedPositive = true;
                } else if (prevPos && !currPos) {
                    input.joystick.customX[xIdx].uncrossedPositive = true;
                }

                bool prevNeg = prevVal <= -t;
                bool currNeg = currVal <= -t;
                if (!prevNeg && currNeg) {
                    input.joystick.customX[xIdx].crossedNegative = true;
                } else if (prevNeg && !currNeg) {
                    input.joystick.customX[xIdx].uncrossedNegative = true;
                }
                xIdx++;
            } else if (thresh.axis == Axis::Y && yIdx < input.joystick.customYCount) {
                bool prevPos = prevVal >= t;
                bool currPos = currVal >= t;
                if (!prevPos && currPos) {
                    input.joystick.customY[yIdx].crossedPositive = true;
                } else if (prevPos && !currPos) {
                    input.joystick.customY[yIdx].uncrossedPositive = true;
                }

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

        prevX = currX;
        prevY = currY;
        prevDir = currDir;

#ifdef DEBUG
        bool info = false;
        static bool wasMoved = false;
        bool nowMoved = joystick.isMoved();


        info |= nowMoved != wasMoved || nowMoved;
        info |= joystickButton->isPressed();
        info |= joystickButton->isReleased();
        info |= joystickButton->isHeld();
        for (int i = 0; i < 5; ++i) 
            info |= analogButtons->isPressed(i);
        for (int i = 0; i < 5; ++i) 
            info |= analogButtons->isHeld(i);
        for (int i = 0; i < 5; ++i) 
            info |= analogButtons->isReleased(i);

        wasMoved = nowMoved;

        if (!info) return;
        Serial.print("JX"+String(joystick.getX())+"Y"+String(joystick.getY()));

        if (nowMoved) {
            Serial.print("m");
        } else {
            Serial.print("c");
        }

        Serial.print("JB:");

        if (joystickButton->isPressed()) {
            Serial.print("P");
        } else if (joystickButton->isReleased()) {
            Serial.print("R");
        } else if (joystickButton->isHeld()) {
            Serial.print("H");
        } else {
            Serial.print("N");
        }
        Serial.print("AP");
        for (int i = 0; i < 5; ++i) 
            Serial.print(analogButtons->isPressed(i));
        Serial.print("AR");
        for (int i = 0; i < 5; ++i) 
            Serial.print(analogButtons->isReleased(i));
        Serial.print("AH");
        for (int i = 0; i < 5; ++i) 
            Serial.print(analogButtons->isHeld(i));
        Serial.print("b");
        // for (uint8_t i = 0; i < 5; ++i) {
        //     Serial.print(input.analogButtons[i].pressed);
        //     Serial.print(input.analogButtons[i].released);
        //     Serial.print(input.analogButtons[i].held);
        // }
        // Serial.print("i");
        // Serial.print(input.joystick.moved);
        // Serial.print(input.joystick.up.entered);
        // Serial.print(input.joystick.up.exited);
        // Serial.print(input.joystick.up.held);
        // Serial.print(input.joystick.down.entered);
        // Serial.print(input.joystick.down.exited);
        // Serial.print(input.joystick.down.held);
        // Serial.print(input.joystick.left.entered);
        // Serial.print(input.joystick.left.exited);
        // Serial.print(input.joystick.left.held);
        // Serial.print(input.joystick.right.entered);
        // Serial.print(input.joystick.right.exited);
        // Serial.print(input.joystick.right.held);
        Serial.print((int)input.joystick.lastUpdated);

        Serial.println();
#endif
    }
}