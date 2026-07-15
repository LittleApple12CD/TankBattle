#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>        // 添加
#include <random>
#include "Utils.h"
#include "Tank.h"
#include "Wall.h"
#include "Explosion.h"
#include "EnemyAI.h"

class Game {
public:
    Game();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void initLevel();
    void togglePvpMode();
    void movePlayer1(int dx, int dy);
    void movePlayer2(int dx, int dy);
    void player1Shoot();
    void player2Shoot();
    std::vector<Wall>& getWalls() { return walls; }
    Tank* getPlayer1() { return player1; }
    Tank* getPlayer2() { return player2; }
    bool isPaused() const { return paused; }
    void setPaused(bool p) { paused = p; }
    bool isGameOver() const { return gameOver; }

private:
    std::vector<Wall> walls;
    std::vector<Explosion> explosions;
    Tank* player1;
    Tank* player2;
    // 改为 list，插入/删除不会使已有元素的指针失效
    std::list<Tank> enemies;        // vector -> list
    std::list<EnemyAI> enemyAIs;    // vector -> list
    float enemySpawnTimer;
    int score;
    bool gameOver;
    bool paused;
    int currentMap;
    bool pvpMode;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    std::uniform_int_distribution<int> intDist;
    void spawnEnemy();
    void handleBulletCollisions();
    void addExplosion(float x, float y);
    void drawUI(sf::RenderWindow& window);
    sf::Font loadFont();
};