#ifndef GAME1_H
#define GAME1_H

#include "Core.h"
#include "Fp.h"

namespace Game1 {
    /* =========================
    Screen configuration
    ========================= */
    #define SCREEN_H 64
    #define SCREEN_W 128

    #define MAX_VIEWING_ANGLE 64

    /* =========================
    Movement
    ========================= */
    #define MOVE_SPEED  (fix16)(0.2f * FIX_ONE)

    /* =========================
    World / map
    ========================= */
    extern const byte MAP_W;
    extern const byte MAP_H;
    extern int16_t GameMap[];

    /* =========================
    Raycasting tables
    ========================= */
    extern fix16 angleStep;

    extern const int STEP_SIZE;
    extern const int STEPS_COUNT;

    extern fix16 cosTable[];
    extern fix16 deltaDistTable[];
    extern int8_t deltaTable[];
    extern fix16 projConstTable[];

    /* =========================
    Player state
    ========================= */
    extern fix16 playerX;
    extern fix16 playerY;
    extern int playerAngleIdx;

    /* =========================
    Precomputed constants
    ========================= */
    extern const fix16 halfScreen;
    extern const fix16 screenHDiv3;

    void precalc();

    bool isWall(int x, int y);

    void raycastDDA(
        fix16 posX,
        fix16 posY,
        int16_t dirIdx,
        fix16& rayOffset,
        fix16& rayDirection,
        bool& hitCorner
    );

    void drawCol(
        Adafruit_SH1106G& display,
        int lineHeight,
        bool hitCorner,
        int col,
        fix16 dist,
        uint16_t color
    );

    void render();

    void update();

    byte getGameState();

    void clean();
}

#endif