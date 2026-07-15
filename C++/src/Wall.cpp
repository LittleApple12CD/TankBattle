#include "Wall.h"

Wall::Wall(float x, float y, float w, float h, bool isSteel)
    : x(x), y(y), w(w), h(h), steel(isSteel), alive(true) {}

sf::FloatRect Wall::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}

void Wall::draw(sf::RenderWindow& window) {
    if (!alive) return;
    sf::RectangleShape rect(sf::Vector2f(w, h));
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(steel ? COLOR_STEEL : COLOR_WALL);
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(1.0f);
    window.draw(rect);
}