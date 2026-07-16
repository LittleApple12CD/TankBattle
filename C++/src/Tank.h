#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include "Utils.h"
#include "Bullet.h"
#include "Wall.h"

// 痕迹点
struct TrailPoint {
    float x, y;
    float age;
};

class Tank {
public:
    Tank(float x, float y, sf::Color color, float speed, bool isPlayer, int playerId);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void move(int dx, int dy, std::vector<Wall>& walls);
    void shoot();
    sf::FloatRect getRect() const;
    sf::Vector2f getCenter() const;
    sf::Vector2f getFirePoint() const;
    bool isAlive() const { return alive; }
    void setAlive(bool a) { alive = a; }
    int getLives() const { return lives; }
    void setLives(int l) { lives = l; }
    void setDirection(float dx, float dy) { dirX = dx; dirY = dy; }
    
    std::list<Bullet> bullets;
    std::list<TrailPoint> trailPoints;  // 行驶痕迹
    int frameCounter;                    // 痕迹帧计数器

private:
    float x, y, w, h;
    sf::Color color;
    float speed;
    bool player;
    int playerId;
    float dirX, dirY;
    int lives;
    float cooldown;
    bool alive;
};