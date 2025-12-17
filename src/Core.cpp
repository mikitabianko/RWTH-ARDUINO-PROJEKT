#include "Core.h"

namespace System {
    Adafruit_SH1106G display(SCREEN_W, SCREEN_H, &Wire, -1);

    DigitalButton joystickButton(JOYSTICK_BUTTON_PIN, 50);
    Joystick joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);
    AnalogButtons analogButtons(BUTTONS_PIN, analogThresholds, 5, 50);

    DigitalButton::DigitalButton(int p, unsigned long delay) : pin(p), debounceDelay(delay) {}

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
                    _isPressedLatch = true; 
                } else if (!currentState && previousState) {
                    _isReleased = true;
                    _isReleasedLatch = true; 
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

    bool DigitalButton::consumePressed() {
        bool value = _isPressedLatch;
        _isPressedLatch = false;
        return value;
    }

    bool DigitalButton::consumeReleased() {
        bool value = _isReleasedLatch;
        _isReleasedLatch = false;
        return value;
    }

    bool DigitalButton::consumePressedEvent() { return consumePressed(); }
    bool DigitalButton::consumeReleasedEvent() { return consumeReleased(); }

    AnalogButtons::AnalogButtons(int p, const ButtonThreshold* thresh, uint8_t count, unsigned long delay)
        : pin(p), thresholds(const_cast<ButtonThreshold*>(thresh)), buttonCount(count), debounceDelay(delay) {
        
        currentState     = 0;
        lastReading      = 0;
        _isPressed       = 0;
        _isReleased      = 0;
        _isPressedLatch  = 0;
        _isReleasedLatch = 0;
    }

    AnalogButtons::~AnalogButtons() {}

    void AnalogButtons::init() {
        AnalogButtons::read();
    }

    inline byte bitSetTo(byte number, byte n, bool x) {
        return (number & ~((byte)1 << n)) | ((byte)x << n);
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

            _isPressedLatch |= _isPressed;
            _isReleasedLatch |= _isReleased;
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

    bool AnalogButtons::consumePressed(uint8_t index) {
        bool value = (_isPressedLatch >> index) & 1;
        _isPressedLatch = 0;
        return value;
    }

    bool AnalogButtons::consumeReleased(uint8_t index) {
        bool value = (_isReleasedLatch >> index) & 1;
        _isReleasedLatch = 0;
        return value;
    }

    bool AnalogButtons::consumePressedEvent(uint8_t index) { return consumePressed(index); }
    bool AnalogButtons::consumeReleasedEvent(uint8_t index) { return consumeReleased(index); }
    
    void Joystick::init() {}

    void Joystick::read() { // to do "isInversed"!
        xValue = 1023 - analogRead(xPin);
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
        while (!displayStarted && (millis() - startTime < 5000)) {  // Timeout 5 сек
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
}

    void handleInput() {
        joystick.read();
        joystickButton.read();
        analogButtons.read();

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
            System::analogButtons.consumePressedEvent(0);
            System::analogButtons.consumeReleasedEvent(0);
            System::joystickButton.consumePressedEvent();
            System::joystickButton.consumeReleasedEvent();
            t1 = now;
        }
        if (now - t2 >= showPeriod) {
            if (wasUpdate) app.show();
            wasUpdate = false;
            t2 = now;
        }
    }

    void setApp(App::App newApp) {
        app = newApp;
        app.init();
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