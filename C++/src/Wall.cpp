#include "Wall.h"

Wall::Wall(float x, float y, float w, float h, bool isSteel)
    : x(x), y(y), w(w), h(h), steel(isSteel), alive(true) {}

sf::FloatRect Wall::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}

void Wall::draw(sf::RenderWindow& window) {
    if (!alive) return;

    sf::Color fillColor = steel ? COLOR_STEEL : COLOR_WALL;
    sf::ConvexShape wall = create16Shape(x, y, w, h, 4.0f, 8.0f, fillColor);
    window.draw(wall);

    sf::ConvexShape border = create16Border(x, y, w, h, 4.0f, 8.0f);
    window.draw(border);
}