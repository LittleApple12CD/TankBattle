#pragma once
#include "Tank.h"
#include <random>

class Game;

class EnemyAI {
public:
    EnemyAI(Tank* tank, Game* game);
    void update(float dt);

private:
    Tank* tank;
    Game* game;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    float dirChangeTimer;
    float shootTimer;
    float dirX, dirY;
    void changeDirection();
    bool canSeePlayer(Tank* player);
    Tank* getNearestPlayer();
    void shootAtNearestPlayer();
};