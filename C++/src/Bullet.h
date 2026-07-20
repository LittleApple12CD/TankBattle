#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Utils.h"

struct TrailParticle {
    float x, y;
    float size;
    float age;
    float lifetime;
};

class Bullet {
public:
    Bullet(float x, float y, float dx, float dy, bool isPlayer, int playerId, sf::Color color);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getRect() const;
    bool isAlive() const { return alive; }
    void setAlive(bool a) { alive = a; }
    bool isPlayerBullet() const { return player; }
    int getPlayerId() const { return playerId; }
    float getX() const { return x; }
    float getY() const { return y; }

    // ===== 这些改为 public，方便 Tank 设置 =====
    float x, y, w, h;
    float dx, dy;
    float speed;
    bool player;
    int playerId;
    sf::Color color;
    bool alive;
    int damage;

    std::vector<TrailParticle> trail;
};