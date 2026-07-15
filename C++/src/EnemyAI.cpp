#include "EnemyAI.h"
#include "Game.h"
#include <cmath>

EnemyAI::EnemyAI(Tank* tank, Game* game)
    : tank(tank), game(game), rng(std::random_device{}()), dist(0.0f, 1.0f),
      dirX(0), dirY(1), dirChangeTimer(AI_DIRECTION_CHANGE + dist(rng)),
      shootTimer(0.5f + dist(rng)) {}

void EnemyAI::update(float dt) {
    tank->update(dt);
    dirChangeTimer -= dt;
    if (dirChangeTimer <= 0) {
        changeDirection();
        dirChangeTimer = AI_DIRECTION_CHANGE + dist(rng) * 1.0f;
    }
    tank->move(static_cast<int>(dirX), static_cast<int>(dirY), game->getWalls());
    shootTimer -= dt;
    if (shootTimer <= 0) {
        if (dist(rng) < AI_SHOOT_CHANCE) shootAtNearestPlayer();
        shootTimer = 0.8f + dist(rng) * 0.5f;
    }
}

void EnemyAI::changeDirection() {
    float dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    int idx = static_cast<int>(dist(rng) * 4);
    if (idx >= 4) idx = 3;
    dirX = dirs[idx][0];
    dirY = dirs[idx][1];
    tank->setDirection(dirX, dirY);
}

bool EnemyAI::canSeePlayer(Tank* player) {
    if (!player || !player->isAlive()) return false;
    sf::Vector2f t = tank->getCenter();
    sf::Vector2f p = player->getCenter();
    float dx = p.x - t.x, dy = p.y - t.y;
    return std::sqrt(dx*dx + dy*dy) < 300.0f;
}

Tank* EnemyAI::getNearestPlayer() {
    Tank* nearest = nullptr;
    float minDist = 1e9f;
    Tank* p1 = game->getPlayer1();
    Tank* p2 = game->getPlayer2();
    if (p1 && p1->isAlive()) {
        sf::Vector2f t = tank->getCenter();
        sf::Vector2f p = p1->getCenter();
        float d = std::sqrt((p.x-t.x)*(p.x-t.x) + (p.y-t.y)*(p.y-t.y));
        if (d < minDist) { minDist = d; nearest = p1; }
    }
    if (p2 && p2->isAlive()) {
        sf::Vector2f t = tank->getCenter();
        sf::Vector2f p = p2->getCenter();
        float d = std::sqrt((p.x-t.x)*(p.x-t.x) + (p.y-t.y)*(p.y-t.y));
        if (d < minDist) { minDist = d; nearest = p2; }
    }
    return nearest;
}

void EnemyAI::shootAtNearestPlayer() {
    Tank* target = getNearestPlayer();
    if (target && target->isAlive()) {
        sf::Vector2f t = tank->getCenter();
        sf::Vector2f p = target->getCenter();
        float dx = p.x - t.x, dy = p.y - t.y;
        float len = std::sqrt(dx*dx + dy*dy);
        if (len > 0) { tank->setDirection(dx/len, dy/len); }
        tank->shoot();
    }
}