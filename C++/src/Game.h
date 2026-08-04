#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <random>
#include <string>
#include "Utils.h"
#include "Tank.h"
#include "Wall.h"
#include "Explosion.h"
#include "EnemyAI.h"
#include "PowerUp.h"
#include "LevelState.h"
#include "LevelData.h"
#include "SaveManager.h"
#include "SoundManager.h"

class Game {
public:
    Game();
    ~Game();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void initLevel();
    void togglePvpMode();
    void toggleSingleMode();
    void movePlayer1(int dx, int dy);
    void movePlayer2(int dx, int dy);
    void player1Shoot();
    void player2Shoot();
    
    // ===== 关卡模式 =====
    void startLevelMode(int level);
    void continueToNext();
    bool isWaitingForEnter() const { return waitingForEnter; }
    bool isVictoryDone() const { return victoryDone; }
    void setVictoryDone(bool b) { victoryDone = b; }
    std::string getGameMode() const { return gameMode; }
    LevelState::Controller* getLevelController() { return levelController; }

    // ===== Getter/Setter =====
    std::vector<Wall>& getWalls() { return walls; }
    Tank* getPlayer1() { return player1; }
    Tank* getPlayer2() { return player2; }
    bool isPaused() const { return paused; }
    void setPaused(bool p) { paused = p; }
    bool isGameOver() const { return gameOver; }
    void setPlayingMode() { menuMode = false; }
    void setMenuMode() { menuMode = true; }
    bool isMenuMode() const { return menuMode; }
    void setSingleMode(bool b) { singleMode = b; }
    void setPvpMode(bool b) { pvpMode = b; }
    void setEnemyCount(int count) { enemyCount = count; }

private:
    // ===== 成员变量 =====
    std::vector<Wall> walls;
    std::vector<Explosion> explosions;
    std::list<PowerUp> powerups;
    Tank* player1;
    Tank* player2;
    std::list<Tank*> enemies;
    std::list<EnemyAI*> enemyAIs;
    SoundManager& soundManager = SoundManager::getInstance();
    
    float enemySpawnTimer;
    int enemyCount;
    int score;
    bool gameOver;
    bool paused;
    int currentMap;
    bool pvpMode;
    bool singleMode;
    bool menuMode;
    bool gameoverPlayed = false;
    
    float powerupTimer = 0.0f;
    float powerupInterval = 10.0f;
    int maxPowerups = 3;
    
    // ===== 关卡模式 =====
    std::string gameMode;
    LevelState::Controller* levelController;
    int level;
    std::string showMessage;
    bool waitingForEnter;
    bool victoryDone;
    
    // ===== 随机数 =====
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    std::uniform_int_distribution<int> intDist;
    
    // ===== 私有方法 =====
    void spawnEnemy();
    void spawnEnemyForLevel();
    void spawnBoss();
    void updateLevelEnemySpawn(float dt);
    void onLevelCleared();
    void onBossDefeated();
    void onGameVictory();
    void onLevelFailed();
    void handleBulletCollisions();
    void addExplosion(float x, float y);
    void drawUI(sf::RenderWindow& window);
    void updatePowerups(float dt);
    void spawnPowerup();
    void applyPowerup(Tank* tank, PowerUp* powerup);
    void explodeAt(float x, float y);
    sf::Font loadFont();
};