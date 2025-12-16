// #include <Arduino.h>
// #include "fp.h"
// #include <Adafruit_SH110X.h>
// #include "game1.h"
// // #include <Adafruit_SSD1306.h>

// // ---
// #define MAX_VIEWING_ANGLE 64

// unsigned long startMillis;  
// unsigned long currentMillis;

// Adafruit_SH1106G display(128, 64, &Wire, -1);
// // Adafruit_SSD1306 display(128, 64, &Wire, -1);

// // ---

// #define SCREEN_H 64
// #define SCREEN_W 128

// #define MAX_VIEWING_ANGLE 64

// #define MOVE_SPEED  (fix16)(0.07f * FIX_ONE) 

// fix16 angleStep = FIX_FROM_FLOAT((float)MAX_VIEWING_ANGLE / SCREEN_W);

// const int STEP_SIZE = SCREEN_W / MAX_VIEWING_ANGLE;
// const int STEPS_COUNT = 360 * STEP_SIZE;

// const char* flor = "%";
// const char* ceiling = "#";

// fix16 playerX = FIX_FROM_INT(4);  
// fix16 playerY = FIX_FROM_INT(7);  
// int playerAngleIdx = 0;

// fix16 cosTable[STEPS_COUNT]; // 720 * 2
// fix16 deltaDistTable[STEPS_COUNT]; // 720 * 2
// int8_t deltaTable[SCREEN_W]; // 128 * 1
// fix16 projConstTable[SCREEN_W]; // 128 * 2

// byte test[SCREEN_W];

// // 3.264 bytes

// const fix16 halfScreen = FIX_FROM_INT(SCREEN_W / 2);
// const fix16 screenHDiv3 = FIX_DIV(FIX_FROM_INT(SCREEN_H), FIX_FROM_INT(3));  

// const byte MAP_W = 16; //8 * sizeof(byte);
// const byte MAP_H = 8;
// int16_t GameMap[MAP_H] = {
//     (int16_t)0b1111111111111111,
//     (int16_t)0b1000000000000001,
//     (int16_t)0b1100011111100001,
//     (int16_t)0b1000000000010001,
//     (int16_t)0b1000000010000001,
//     (int16_t)0b1000100010000001,
//     (int16_t)0b1000000010000001,
//     (int16_t)0b1111111111111111
// };

// void precalc() {
//     for (int i = 0; i < STEPS_COUNT; ++i) {
//         float angleRad = (i * 2 * M_PI) / STEPS_COUNT;
//         cosTable[i] = FIX_FROM_FLOAT(cosf(angleRad));

//         deltaDistTable[i] = FIX_ABS(FIX_DIV(FIX_ONE, (FIX_ABS(cosTable[i]) < 10) ? (cosTable[i] >= 0 ? 10 : -10) : cosTable[i]));
//     }
//     for (int col = 0; col < SCREEN_W; ++col) {
//         fix16 relCol = FIX_SUB(FIX_FROM_INT(col), halfScreen);
//         fix16 relAngleDeg = FIX_MUL(relCol, angleStep);
//         int delta = FIX_TO_INT(FIX_MUL(relAngleDeg, FIX_FROM_INT(STEP_SIZE)));
//         deltaTable[col] = delta;
//         fix16 cosVal = cosTable[(deltaTable[col] + STEPS_COUNT) % STEPS_COUNT];
//         projConstTable[col] = FIX_DIV(halfScreen, cosVal);  
//     }
// }

// bool isWall(int x, int y) {
//     if (y < 0 || y >= MAP_W || x < 0 || x >= MAP_H) return true;
//     return (GameMap[x] & (1 << (MAP_W - y - 1))) != 0;
// }

// inline void raycastDDA(fix16 posX, fix16 posY, int16_t dirIdx, fix16& rayOffset, fix16& rayDirection, bool& hitCorner) {
//     int16_t dirIdxSin = (dirIdx + 540);
//     dirIdxSin -= (dirIdxSin >= STEPS_COUNT) ? STEPS_COUNT : 0;
//     dirIdxSin += (dirIdxSin < 0) ? STEPS_COUNT : 0;

//     fix16 dirX = cosTable[dirIdx];
//     fix16 dirY = cosTable[dirIdxSin];

//     if (FIX_ABS(dirX) < 10) dirX = (dirX >= 0 ? 10 : -10);
//     if (FIX_ABS(dirY) < 10) dirY = (dirY >= 0 ? 10 : -10);

//     int mapX = FIX_TO_INT(posX);
//     int mapY = FIX_TO_INT(posY);

//     fix16 deltaDistX = deltaDistTable[dirIdx];  
//     fix16 deltaDistY = deltaDistTable[dirIdxSin]; 

//     int stepX = (dirX < 0) ? -1 : 1;
//     int stepY = (dirY < 0) ? -1 : 1;

//     fix16 sideDistX = (dirX < 0)
//         ? FIX_MUL(posX - FIX_FROM_INT(mapX), deltaDistX)
//         : FIX_MUL(FIX_FROM_INT(mapX + 1) - posX, deltaDistX);

//     fix16 sideDistY = (dirY < 0)
//         ? FIX_MUL(posY - FIX_FROM_INT(mapY), deltaDistY)
//         : FIX_MUL(FIX_FROM_INT(mapY + 1) - posY, deltaDistY);

//     int side;
//     while (1) {
//         if (sideDistX < sideDistY) {
//             sideDistX += deltaDistX;
//             mapX += stepX;
//             side = 0;
//         } else {
//             sideDistY += deltaDistY;
//             mapY += stepY;
//             side = 1;
//         }
//         if (isWall(mapX, mapY)) break;
//     }

//     if (side == 0) {
//         fix16 wallX = FIX_FROM_INT(mapX) + (stepX > 0 ? 0 : FIX_ONE);
//         rayOffset = wallX - posX;
//         rayDirection = dirX;
//         // return FIX_DIV(wallX - posX, dirX); 
//     } else {
//         fix16 wallY = FIX_FROM_INT(mapY) + (stepY > 0 ? 0 : FIX_ONE);
//         rayOffset = wallY - posY;
//         rayDirection = dirY;
//         // return FIX_DIV(wallY - posY, dirY); 
//         // hitCorner = true;
//     }

//     hitCorner = false;
//     fix16 eps = 5;
//     if (side == 0) {
//         fix16 ratio = FIX_DIV(dirY, dirX);
//         fix16 deltaY = FIX_MUL(rayOffset, ratio);
//         fix16 hitY = posY + deltaY;
//         fix16 frac = hitY & (FIX_ONE - 1);
//         if (frac <= eps || frac >= (FIX_ONE - eps)) hitCorner = true;
//     } else {
//         fix16 ratio = FIX_DIV(dirX, dirY);
//         fix16 deltaX = FIX_MUL(rayOffset, ratio);
//         fix16 hitX = posX + deltaX;
//         fix16 frac = hitX & (FIX_ONE - 1);
//         if (frac <= eps || frac >= (FIX_ONE - eps)) hitCorner = true;
//     }
// }

// void drawCol(int lineHeight, bool hitCorner, int col, fix16 dist, uint16_t color) {
//     int drawStart = (SCREEN_H >> 1) - (lineHeight >> 1); 
//     int drawEnd = (SCREEN_H >> 1) + (lineHeight >> 1);  

//     // if (drawStart < 0) drawStart = 0;
//     if (drawEnd >= SCREEN_H) drawEnd = SCREEN_H - 1;
    
//     int shadeLevel = min(5, max(FIX_TO_INT(dist << 2), 2));  // Example: 0- close, higher- far
//     for (int y = drawStart; y < drawEnd; y += shadeLevel) {
//         display.drawPixel(col, y, color);
//     }

//     if (hitCorner) display.drawFastVLine(col, drawStart, lineHeight, color);

//     display.drawPixel(col, drawStart, color);
//     display.drawPixel(col, drawEnd, color);
// }

// void render() {
//     for (int col = 0; col < SCREEN_W; col++) {
//         int rayIdx = (playerAngleIdx + deltaTable[col]);
//         rayIdx -= (rayIdx >= STEPS_COUNT) ? STEPS_COUNT : 0;
//         rayIdx += (rayIdx < 0) ? STEPS_COUNT : 0;

//         fix16 a, b;
//         bool hitCorner = false;


//         raycastDDA(playerX, playerY, rayIdx, a, b, hitCorner);
//         fix16 dist = FIX_DIV(a, b);

//         int relAngle = deltaTable[col];
//         relAngle -= (relAngle >= STEPS_COUNT) ? STEPS_COUNT : 0;
//         relAngle += (relAngle < 0) ? STEPS_COUNT : 0;

//         fix16 heightFactor = FIX_DIV(projConstTable[col], dist); 

//         int lineHeight = FIX_TO_INT(heightFactor);

//         // if (test[col] != lineHeight) {

//         //     drawCol(test[col], hitCorner, col, dist, SH110X_BLACK);
//         //     test[col] = lineHeight;
//             drawCol(lineHeight, hitCorner, col, dist, SH110X_WHITE);
//         // }
//     }
// }

// void update(int x, int y) {
//     int playerAngleIdxSin = (playerAngleIdx + 540);
//     playerAngleIdxSin -= (playerAngleIdxSin >= STEPS_COUNT) ? STEPS_COUNT : 0;
//     playerAngleIdxSin += (playerAngleIdxSin < 0) ? STEPS_COUNT : 0;
//     playerX += FIX_MUL(FIX_MUL(MOVE_SPEED, cosTable[playerAngleIdx]), FIX_FROM_INT(x));
//     playerY += FIX_MUL(FIX_MUL(MOVE_SPEED, cosTable[playerAngleIdxSin]), FIX_FROM_INT(x));

//     playerAngleIdx = (playerAngleIdx + y * 7);
//     playerAngleIdx += (playerAngleIdx < 0) ? STEPS_COUNT : 0;
//     playerAngleIdx -= (playerAngleIdx >= STEPS_COUNT) ? STEPS_COUNT : 0;
// }

// int norm(int n) {
//     if (n > 700) return -1;
//     if (n < 300) return 1;
//     return 0;
// }

// void setup() {
//     // while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C));
//     while (!display.begin(0x3C));

//     display.clearDisplay();
//     display.display();

//     precalc();

//     render();

//     display.display();

//     Serial.begin(9600);
// }

// void loop() {
//     currentMillis = millis();  
//     if (currentMillis - startMillis >= 50)  {
//         auto test = micros();
//         int x = norm(analogRead(1));
//         int y = norm(analogRead(0));

//         if (x != 0 || y != 0) {
//             display.clearDisplay();
//             update(x, y);
//             render();
//             display.display();
//         }
//         Serial.println(micros() - test);
//     }
// }
