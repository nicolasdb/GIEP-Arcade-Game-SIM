#ifndef RAIN_SYSTEM_H
#define RAIN_SYSTEM_H

#include "config.h"
#include "MatrixConfig.h"

struct Raindrop {
    int x;
    int y;
    bool active;
};

class RainSystem {
public:
    RainSystem(MatrixConfig& matrix);
    void init();
    void update(GameState currentState);

private:
    MatrixConfig& matrix;
    Raindrop raindrops[MAX_RAINDROPS];

    void spawnRaindrop(CRGB color);
    bool isBlockZone(int x, int y);
    void updateRaindrops(CRGB color, int fallSpeed, int spawnChance);
};

#endif // RAIN_SYSTEM_H