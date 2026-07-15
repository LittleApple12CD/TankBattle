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

const std::string MAP_NAMES[] = {"空地", "十字", "迷宫", "碉堡", "对称"};

inline float clampf(float val, float min, float max) {
    return std::max(min, std::min(max, val));
}
inline int clampi(int val, int min, int max) {
    return std::max(min, std::min(max, val));
}