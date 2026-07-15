#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include "Utils.h"
#include "Bullet.h"
#include "Wall.h"

class Tank {
public:
    Tank(float x, float y, sf::Color color, float speed, bool isPlayer, int playerId);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void move(int dx, int dy, std::vector<Wall>& walls);
    void shoot();  // 改为 void，不返回指针
    sf::FloatRect getRect() const;
    sf::Vector2f getCenter() const;
    sf::Vector2f getFirePoint() const;
    bool isAlive() const { return alive; }
    void setAlive(bool a) { alive = a; }
    int getLives() const { return lives; }
    void setLives(int l) { lives = l; }
    void setDirection(float dx, float dy) { dirX = dx; dirY = dy; }
    
    std::list<Bullet> bullets;  // 改为 list

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