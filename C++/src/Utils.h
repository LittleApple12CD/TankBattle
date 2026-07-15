#pragma once
#include <SFML/Graphics.hpp>
#include <string>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
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
const float BULLET_SPEED = 360.0f;
const int MAX_BULLETS = 3;

const int ENEMY_COUNT = 3;
const float ENEMY_SPAWN_INTERVAL = 4.0f;
const float AI_DIRECTION_CHANGE = 2.0f;
const float AI_SHOOT_CHANCE = 0.2f;

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
// 创建 16 边形（带圆角效果）
// ============================================
inline sf::ConvexShape create16Shape(float x, float y, float w, float h,
                                      float cornerCut, float midLength,
                                      sf::Color fillColor,
                                      sf::Color outlineColor = sf::Color::White,
                                      float outlineThickness = 1.0f) {
    sf::ConvexShape shape;
    shape.setPointCount(16);

    // 顺时针：从顶部中间开始
    shape.setPoint(0,  sf::Vector2f(x + cornerCut, y));
    shape.setPoint(1,  sf::Vector2f(x + w/2 - midLength/2, y));
    shape.setPoint(2,  sf::Vector2f(x + w/2 + midLength/2, y));
    shape.setPoint(3,  sf::Vector2f(x + w - cornerCut, y));
    shape.setPoint(4,  sf::Vector2f(x + w, y + cornerCut));
    shape.setPoint(5,  sf::Vector2f(x + w, y + h/2 - midLength/2));
    shape.setPoint(6,  sf::Vector2f(x + w, y + h/2 + midLength/2));
    shape.setPoint(7,  sf::Vector2f(x + w, y + h - cornerCut));
    shape.setPoint(8,  sf::Vector2f(x + w - cornerCut, y + h));
    shape.setPoint(9,  sf::Vector2f(x + w/2 + midLength/2, y + h));
    shape.setPoint(10, sf::Vector2f(x + w/2 - midLength/2, y + h));
    shape.setPoint(11, sf::Vector2f(x + cornerCut, y + h));
    shape.setPoint(12, sf::Vector2f(x, y + h - cornerCut));
    shape.setPoint(13, sf::Vector2f(x, y + h/2 + midLength/2));
    shape.setPoint(14, sf::Vector2f(x, y + h/2 - midLength/2));
    shape.setPoint(15, sf::Vector2f(x, y + cornerCut));

    shape.setFillColor(fillColor);
    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(outlineThickness);

    return shape;
}

// ============================================
// 创建 16 边形的边框（比主体大一圈）
// ============================================
inline sf::ConvexShape create16Border(float x, float y, float w, float h,
                                       float cornerCut, float midLength,
                                       sf::Color outlineColor = sf::Color::White,
                                       float thickness = 1.0f) {
    float offset = 1.0f;

    sf::ConvexShape border;
    border.setPointCount(16);

    border.setPoint(0,  sf::Vector2f(x + cornerCut - offset, y - offset));
    border.setPoint(1,  sf::Vector2f(x + w/2 - midLength/2, y - offset));
    border.setPoint(2,  sf::Vector2f(x + w/2 + midLength/2, y - offset));
    border.setPoint(3,  sf::Vector2f(x + w - cornerCut + offset, y - offset));
    border.setPoint(4,  sf::Vector2f(x + w + offset, y + cornerCut - offset));
    border.setPoint(5,  sf::Vector2f(x + w + offset, y + h/2 - midLength/2));
    border.setPoint(6,  sf::Vector2f(x + w + offset, y + h/2 + midLength/2));
    border.setPoint(7,  sf::Vector2f(x + w + offset, y + h - cornerCut + offset));
    border.setPoint(8,  sf::Vector2f(x + w - cornerCut + offset, y + h + offset));
    border.setPoint(9,  sf::Vector2f(x + w/2 + midLength/2, y + h + offset));
    border.setPoint(10, sf::Vector2f(x + w/2 - midLength/2, y + h + offset));
    border.setPoint(11, sf::Vector2f(x + cornerCut - offset, y + h + offset));
    border.setPoint(12, sf::Vector2f(x - offset, y + h - cornerCut + offset));
    border.setPoint(13, sf::Vector2f(x - offset, y + h/2 + midLength/2));
    border.setPoint(14, sf::Vector2f(x - offset, y + h/2 - midLength/2));
    border.setPoint(15, sf::Vector2f(x - offset, y + cornerCut - offset));

    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(outlineColor);
    border.setOutlineThickness(thickness);

    return border;
}