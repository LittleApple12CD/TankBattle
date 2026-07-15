#include "Explosion.h"

const sf::Color Explosion::colors[] = {
    sf::Color(255, 255, 200),
    sf::Color(255, 200, 100),
    sf::Color(255, 150, 50),
    sf::Color(255, 100, 20),
    sf::Color(200, 50, 0)
};

Explosion::Explosion(float x, float y)
    : x(x), y(y), radius(5.0f), maxRadius(30.0f), age(0), lifetime(0.5f), alive(true) {}

void Explosion::update(float dt) {
    age += dt;
    float progress = age / lifetime;
    radius = maxRadius * progress;
    if (age >= lifetime) alive = false;
}

void Explosion::draw(sf::RenderWindow& window) {
    if (!alive) return;
    float progress = age / lifetime;
    int idx = static_cast<int>(progress * 5);
    if (idx >= 5) idx = 4;
    sf::CircleShape shape(radius);
    shape.setPosition(sf::Vector2f(x - radius, y - radius));
    shape.setFillColor(colors[idx]);
    window.draw(shape);
}