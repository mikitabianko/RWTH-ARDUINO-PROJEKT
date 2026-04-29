#include "../common/IDigitalButton.h"

namespace Drivers {
    class DigitalButton_Driver : public IDigitalButton {
    private:
        int _pin;
        unsigned long _debounceDelay;
        unsigned long _lastDebounceTime = 0;
        
        bool _currentState = false;
        bool _lastReading = false;
        bool _isPressed = false;
        bool _isReleased = false;

    public:
        DigitalButton_Driver(int pin, unsigned long debounce = 50);

        void init() override;

        void read() override;

        bool isPressed() override;
        bool isReleased() override;
        bool isHeld() override;
    };
}