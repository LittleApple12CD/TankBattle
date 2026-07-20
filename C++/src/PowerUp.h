#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.h"

class PowerUp {
public:
    PowerUp(float x, float y, char type);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getRect() const;
    bool isAlive() const { return alive; }
    void setAlive(bool a) { alive = a; }
    char getType() const { return type; }

private:
    float x, y, w, h;
    char type;  // 'S', 'P', 'H', 'T'
    bool alive;
    sf::Font font;

    sf::Color getColor() const;
};