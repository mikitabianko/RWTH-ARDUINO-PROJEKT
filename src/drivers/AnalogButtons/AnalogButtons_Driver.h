// #include "../common/IAnalogButtons.h"

// namespace Drivers {
//     class AnalogButtons {
//     private:
//         int pin;
//         const ButtonThreshold* thresholds;
//         uint8_t buttonCount;
//         const unsigned long debounceDelay;

//         byte currentState;
//         byte lastReading;
//         byte _isPressed; 
//         byte _isReleased;

//         unsigned long lastDebounceTime = 0;
//         int lastAnalogValue = 0;

//     public:
//         AnalogButtons(int p, const ButtonThreshold* thresh, uint8_t count, unsigned long delay = 50);
//         ~AnalogButtons();

//         void init();
//         void read();

//         bool isPressed(uint8_t index) const;
//         bool isReleased(uint8_t index) const;
//         bool isHeld(uint8_t index) const;

//         int getRawValue() const;
//         uint8_t getButtonCount() const;
//     };
// }