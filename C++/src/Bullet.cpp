#include "Bullet.h"

Bullet::Bullet(float x, float y, float dx, float dy, bool isPlayer, int pid, sf::Color col)
    : x(x - BULLET_SIZE/2.0f), y(y - BULLET_SIZE/2.0f),
      w(BULLET_SIZE), h(BULLET_SIZE),
      dx(dx), dy(dy),
      speed(BULLET_SPEED),   // 关键：初始化 speed
      player(isPlayer), playerId(pid), color(col), alive(true) {}

void Bullet::update(float dt) {
    // 加安全保护
    if (!alive) return;
    
    x += dx * speed * dt;
    y += dy * speed * dt;
    if (x < -20 || x > WINDOW_WIDTH + 20 || y < -20 || y > WINDOW_HEIGHT + 20)
        alive = false;
}

void Bullet::draw(sf::RenderWindow& window) {
    if (!alive) return;
    sf::CircleShape shape(w / 2.0f);
    shape.setPosition(sf::Vector2f(x, y));
    shape.setFillColor(color);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(1.0f);
    window.draw(shape);
}

sf::FloatRect Bullet::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}