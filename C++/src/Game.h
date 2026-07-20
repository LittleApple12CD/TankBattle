#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <random>
#include "Utils.h"
#include "Tank.h"
#include "Wall.h"
#include "Explosion.h"
#include "EnemyAI.h"
#include "PowerUp.h"

class Game {
public:
    Game();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void initLevel();
    void togglePvpMode();
    void toggleSingleMode();   // 声明
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
    void setPlayingMode() { menuMode = false; }
    void setMenuMode() { menuMode = true; }
    bool isMenuMode() const { return menuMode; }
    void updatePowerups(float dt);
    void spawnPowerup();
    void applyPowerup(Tank* tank, PowerUp* powerup);
    void explodeAt(float x, float y);

    void setSingleMode(bool b) { singleMode = b; }
    void setPvpMode(bool b) { pvpMode = b; }

private:
    std::list<PowerUp> powerups;
    std::vector<Wall> walls;
    std::vector<Explosion> explosions;
    Tank* player1;
    Tank* player2;
    std::list<Tank> enemies;
    std::list<EnemyAI> enemyAIs;
    float powerupTimer = 0.0f;
    float powerupInterval = 10.0f;
    int maxPowerups = 3;
    float enemySpawnTimer;
    int enemyCount;
    int score;
    bool gameOver;
    bool paused;
    int currentMap;
    bool pvpMode;
    bool singleMode;          // 新增
    bool menuMode;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    std::uniform_int_distribution<int> intDist;
    void spawnEnemy();
    void handleBulletCollisions();
    void addExplosion(float x, float y);
    void drawUI(sf::RenderWindow& window);
    sf::Font loadFont();
};