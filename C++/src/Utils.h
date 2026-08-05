#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include "ConfigManager.h"

// ============================================
// 这些值现在从 config.ini 读取
// ============================================
inline int getWindowWidth() { return ConfigManager::getInstance().getWindowWidth(); }
inline int getWindowHeight() { return ConfigManager::getInstance().getWindowHeight(); }
inline int getFPS() { return ConfigManager::getInstance().getFPS(); }
inline int getTankSize() { return ConfigManager::getInstance().getTankSize(); }
inline int getTankSpeed() { return ConfigManager::getInstance().getTankSpeed(); }
inline int getPlayerLives() { return ConfigManager::getInstance().getPlayerLives(); }
inline float getShotCooldown() { return ConfigManager::getInstance().getShotCooldown(); }
inline int getBulletSize() { return ConfigManager::getInstance().getBulletSize(); }
inline float getBulletSpeed() { return ConfigManager::getInstance().getBulletSpeed(); }
inline int getMaxBullets() { return ConfigManager::getInstance().getMaxBullets(); }
inline int getEnemyCount() { return ConfigManager::getInstance().getEnemyCount(); }
inline float getEnemySpawnInterval() { return ConfigManager::getInstance().getEnemySpawnInterval(); }
inline float getAIDirectionChange() { return ConfigManager::getInstance().getAIDirectionChange(); }
inline float getAIShootChance() { return ConfigManager::getInstance().getAIShootChance(); }
inline float getPowerUpSpawnInterval() { return ConfigManager::getInstance().getPowerUpSpawnInterval(); }
inline int getMaxPowerups() { return ConfigManager::getInstance().getMaxPowerups(); }

// ============================================
// 宏定义 - 改为调用函数
// ============================================
#define WINDOW_WIDTH getWindowWidth()
#define WINDOW_HEIGHT getWindowHeight()
#define FPS getFPS()
#define TANK_SIZE getTankSize()
#define TANK_SPEED getTankSpeed()
#define PLAYER_LIVES getPlayerLives()
#define SHOT_COOLDOWN getShotCooldown()
#define BULLET_SIZE getBulletSize()
#define BULLET_SPEED getBulletSpeed()
#define MAX_BULLETS getMaxBullets()
#define ENEMY_COUNT getEnemyCount()
#define ENEMY_SPAWN_INTERVAL getEnemySpawnInterval()
#define AI_DIRECTION_CHANGE getAIDirectionChange()
#define AI_SHOOT_CHANCE getAIShootChance()
#define POWERUP_SPAWN_INTERVAL getPowerUpSpawnInterval()
#define MAX_POWERUPS getMaxPowerups()

// ============================================
// 网格尺寸 - 改为函数
// ============================================
const int GRID_SIZE = 13;
const int CELL_SIZE = 50;

inline int getGridOffsetX() {
    return (getWindowWidth() - GRID_SIZE * CELL_SIZE) / 2;
}
inline int getGridOffsetY() {
    return (getWindowHeight() - GRID_SIZE * CELL_SIZE) / 2;
}

#define GRID_OFFSET_X getGridOffsetX()
#define GRID_OFFSET_Y getGridOffsetY()

// ============================================
// MOVE_STEP - 保持不变
// ============================================
const float MOVE_STEP = 2.0f;  // 确保这行存在且没有被注释

// ============================================
// 音效相关
// ============================================
inline bool SOUND_ENABLED = true;
inline float SOUND_VOLUME = ConfigManager::getInstance().getVolume();
inline bool SOUND_MUTED = ConfigManager::getInstance().isMuted();

// ============================================
// 颜色常量
// ============================================
const sf::Color COLOR_BG(20, 20, 30);
const sf::Color COLOR_GRID(30, 30, 40);
const sf::Color COLOR_PLAYER1(0, 200, 80);
const sf::Color COLOR_PLAYER2(255, 180, 50);
const sf::Color COLOR_ENEMY(200, 50, 50);
const sf::Color COLOR_BULLET_P1(100, 255, 100);
const sf::Color COLOR_BULLET_P2(255, 220, 100);
const sf::Color COLOR_BULLET_ENEMY(255, 100, 100);
const sf::Color COLOR_WALL(150, 150, 150);
const sf::Color COLOR_STEEL(180, 180, 200);
const sf::Color COLOR_TEXT(255, 255, 255);
const sf::Color COLOR_TEXT_DIM(150, 150, 160);
const sf::Color COLOR_PVP(255, 100, 100);

// ============================================
// 工具函数
// ============================================
inline float clampf(float val, float min, float max) {
    return std::max(min, std::min(max, val));
}
inline int clampi(int val, int min, int max) {
    return std::max(min, std::min(max, val));
}

// 圆角矩形
inline sf::ConvexShape createRoundedRect(float x, float y, float w, float h,
                                          float radius,
                                          sf::Color fillColor,
                                          sf::Color outlineColor = sf::Color::White,
                                          float outlineThickness = 1.0f) {
    radius = std::min(radius, std::min(w, h) / 2.0f);
    
    // 使用 24 个顶点（每个角 6 个点）
    const int seg = 6;
    const int totalPoints = seg * 4;
    sf::ConvexShape shape;
    shape.setPointCount(totalPoints);
    
    const float pi = 3.14159265358979323846f;
    
    // 圆心坐标
    float cx[4] = {x + radius, x + w - radius, x + w - radius, x + radius};
    float cy[4] = {y + radius, y + radius, y + h - radius, y + h - radius};
    
    // 起始角度：左上=180°, 右上=270°, 右下=0°, 左下=90°
    float startAngles[4] = {pi, pi * 1.5f, 0.0f, pi * 0.5f};
    
    int idx = 0;
    for (int corner = 0; corner < 4; ++corner) {
        for (int i = 0; i < seg; ++i) {
            float angle = startAngles[corner] + (float)i / (float)seg * (pi * 0.5f);
            float px = cx[corner] + radius * std::cos(angle);
            float py = cy[corner] + radius * std::sin(angle);
            shape.setPoint(idx++, sf::Vector2f(px, py));
        }
    }
    
    shape.setFillColor(fillColor);
    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(outlineThickness);
    
    return shape;
}

// 音效路径
const std::string SOUND_PATH = "assets/sounds/";
const std::string SOUND_NAMES[] = {"shoot", "explode", "powerup", "victory", "gameover"};
const std::string SOUND_FILES[] = {"shoot.wav", "explode.wav", "powerup.wav", "victory.wav", "gameover.wav"};
const int SOUND_COUNT = 5;