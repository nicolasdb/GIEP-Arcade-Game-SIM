#ifndef SCENE_H
#define SCENE_H

#include "config.h"
#include "MatrixConfig.h"

class Scene {
public:
    Scene(MatrixConfig& mainMatrix, MatrixConfig& secMatrix);
    void init();
    void update(GameState currentState);

private:
    MatrixConfig& mainMatrix;
    MatrixConfig& secMatrix;

    void drawWaitingScene(bool isRaining);
    void drawRainScene(int fallSpeed);
    void drawWinScene();
    void drawFloodScene();
    void drawPollutionScene();
    void updateSecondaryMatrix(GameState state);
};

#endif // SCENE_H