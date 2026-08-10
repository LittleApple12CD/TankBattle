// src/Tank.h
#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include <map>
#include <string>
#include "Utils.h"
#include "Bullet.h"
#include "Wall.h"

struct TrailPoint {
    float x, y;
    float age;
};

class Tank {
public:
    Tank(float x, float y, sf::Color color, float speed, bool isPlayer, int playerId);
    virtual ~Tank() = default;
    
    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& window);
    virtual void move(int dx, int dy, std::vector<Wall>& walls);
    virtual Bullet* shoot();
    virtual sf::FloatRect getRect() const;
    virtual sf::Vector2f getCenter() const;
    virtual sf::Vector2f getFirePoint() const;
    
    virtual void drawBossHealthBar(sf::RenderWindow& window);
    
    bool isAlive() const { return alive; }
    void setAlive(bool a) { alive = a; }
    int getLives() const { return lives; }
    void setLives(int l) { lives = l; }
    int getMaxHp() const { return maxHp; }
    void setMaxHp(int hp) { maxHp = hp; }
    void setDirection(float dx, float dy) { dirX = dx; dirY = dy; }
    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float nx, float ny) { x = nx; y = ny; }
    float getSpeed() const { return speed; }
    void setSpeed(float s) { speed = s; }

    void addEffect(const std::string& type, float duration) {
        effects[type] = duration;
    }
    
    float x, y, w, h;
    sf::Color color;
    float speed;
    bool player;
    int playerId;
    float dirX, dirY;
    int lives;
    float cooldown;
    bool alive;
    bool isBoss = false;
    
    std::list<Bullet> bullets;
    std::list<TrailPoint> trailPoints;
    std::list<Bullet>& getBullets() { return bullets; }
    int frameCounter;

    std::map<std::string, float> effects;

    bool isProtected() const {
        return effects.find("protection") != effects.end();
    }

    float getSpeedMultiplier() const {
        return (effects.find("speed") != effects.end()) ? 1.5f : 1.0f;
    }

    float getBulletSpeedMultiplier() const {
        return (effects.find("speed") != effects.end()) ? 1.5f : 1.0f;
    }

    float getSizeScale() const {
        return (effects.find("strength") != effects.end()) ? 1.5f : 1.0f;
    }

    int getBulletDamage() const {
        return (effects.find("strength") != effects.end()) ? 2 : 1;
    }

protected:
    int maxHp = 1;
};