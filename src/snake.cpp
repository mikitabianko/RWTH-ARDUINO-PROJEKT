// #include "Arduino.h"
// #include "Adafruit_SH110X.h"

// namespace Snake {
//     struct Point {
//         byte X, Y;
//     };
//     struct Vector {
//         byte X, Y;
//     };

//     Vector dir = { 0, 0 };

//     const int _size = 24 * 64;
//     int size = 0;
//     int headIdx = 0;
//     Point* snake;

//     void init() {
//         // snake = (Point*)malloc(sizeof(Point) * _size);
//         // for (int i = 0; i < _size; ++i) {
//         //     snake[i] = { 255, 255 };
//         // }
//         // size = 3;
//         // headIdx = 2;
//         // snake[0] = { 32, 12 };
//         // snake[1] = { 31, 12 };
//         // snake[2] = { 30, 12 };
//     }

//     void draw(Adafruit_SH1106G& display, int screen_w, int screen_h) {
//         display.clearDisplay();

//         // int a = (headIdx - size + 1 + _size) % _size;
//         // int b = (headIdx + _size) % _size;

//         // Serial.print(a);
//         // Serial.print(' ');
//         // Serial.println(b);

//         // for (int i = a; i != b; i = (i + 1) % _size) {
//         //     display.drawPixel(snake[i].X * 2, snake[i].Y * 2, SH110X_WHITE);
//         // }

//         display.display();
//     }

//     int normalize(int n) {
//         if (n > 700) return -1;
//         if (n < 300) return 1;
//         return 0;
//     }

//     void update(int joystickX, int joystickY, int joystickButtonState) {
//         byte x = normalize(joystickX);
//         byte y = normalize(joystickY);

//         if (x != y) {
//             dir = { y, x };
//         }


//     }

//     byte getGameState() {
//         return 0;
//     }

//     void clean() {

//     }
// }