#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.h"

class Explosion {
public:
    Explosion(float x, float y);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool isAlive() const { return alive; }

private:
    float x, y;
    float radius;
    float maxRadius;
    float age;
    float lifetime;
    bool alive;
    static const sf::Color colors[];
};