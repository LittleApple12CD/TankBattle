#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.h"

class Wall {
public:
    Wall(float x, float y, float w, float h, bool isSteel);
    void draw(sf::RenderWindow& window);
    bool isAlive() const { return alive; }
    void setAlive(bool a) { alive = a; }
    sf::FloatRect getRect() const;
    bool isSteel() const { return steel; }

private:
    float x, y, w, h;
    bool steel;
    bool alive;
};