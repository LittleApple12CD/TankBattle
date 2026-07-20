#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
const int FPS = 60;

const int GRID_SIZE = 13;
const int CELL_SIZE = 50;
const int GRID_OFFSET_X = (WINDOW_WIDTH - GRID_SIZE * CELL_SIZE) / 2;
const int GRID_OFFSET_Y = (WINDOW_HEIGHT - GRID_SIZE * CELL_SIZE) / 2;

const int TANK_SIZE = 36;
const int TANK_SPEED = 6;
const int PLAYER_LIVES = 3;
const float SHOT_COOLDOWN = 0.5f;

const int BULLET_SIZE = 8;
const float BULLET_SPEED = 480.0f;
const int MAX_BULLETS = 3;

const int ENEMY_COUNT = 4;
const float ENEMY_SPAWN_INTERVAL = 4.0f;
const float AI_DIRECTION_CHANGE = 2.0f;
const float AI_SHOOT_CHANCE = 0.4f;

const float MOVE_STEP = 2.0f;

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

const std::string MAP_NAMES[] = {"Empty", "Cross", "Maze", "Bunker", "Sym"};

inline float clampf(float val, float min, float max) {
    return std::max(min, std::min(max, val));
}
inline int clampi(int val, int min, int max) {
    return std::max(min, std::min(max, val));
}

// ============================================
// 创建圆角矩形（精确版，20边形，更平滑）
// ============================================
inline sf::ConvexShape createRoundedRect(float x, float y, float w, float h,
                                          float radius,
                                          sf::Color fillColor,
                                          sf::Color outlineColor = sf::Color::White,
                                          float outlineThickness = 1.0f) {
    radius = std::min(radius, std::min(w, h) / 2.0f);
    int segments = 5; // 每个角5个点，总共20个点，更平滑
    int totalPoints = segments * 4;
    sf::ConvexShape shape;
    shape.setPointCount(totalPoints);
    float pi = 3.14159265f;

    int idx = 0;
    // 四个角的圆心和起始角度（逆时针）
    float centers[4][2] = {
        {x + radius, y + radius},          // 左上
        {x + w - radius, y + radius},      // 右上
        {x + w - radius, y + h - radius},  // 右下
        {x + radius, y + h - radius}       // 左下
    };
    float startAngles[4] = {pi, pi * 1.5f, 0.0f, pi * 0.5f};

    for (int corner = 0; corner < 4; ++corner) {
        float cx = centers[corner][0];
        float cy = centers[corner][1];
        float startAngle = startAngles[corner];
        for (int i = 0; i < segments; ++i) {
            float angle = startAngle + (i / (float)segments) * pi * 0.5f;
            float px = cx + radius * std::cos(angle);
            float py = cy + radius * std::sin(angle);
            shape.setPoint(idx++, sf::Vector2f(px, py));
        }
    }

    shape.setFillColor(fillColor);
    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(outlineThickness);
    return shape;
}