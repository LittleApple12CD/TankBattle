#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.h"

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

private:
    float x, y, w, h;
    float dx, dy;
    float speed;
    bool player;
    int playerId;
    sf::Color color;
    bool alive;
};