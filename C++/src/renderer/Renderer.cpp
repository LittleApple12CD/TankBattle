// src/renderer/Renderer.cpp
#include "Renderer.h"
#include "../Tank.h"
#include "../Bullet.h"
#include "../Wall.h"
#include "../PowerUp.h"
#include "../resource/TextureManager.h"
#include "../Utils.h"
#include <cmath>

Renderer& Renderer::getInstance() {
    static Renderer instance;
    return instance;
}

// ============================================================
// 坦克绘制
// ============================================================

bool Renderer::drawTankWithTexture(sf::RenderWindow& window, Tank* tank,
                                   const std::string& entityId,
                                   int wDraw, int hDraw, int xDraw, int yDraw) {
    auto texture = TextureManager::getInstance().getEntityTexture(entityId);
    if (!texture) return false;

    sf::Sprite sprite(*texture);
    
    sf::Vector2f center = tank->getCenter();
    sprite.setPosition(center);
    
    sf::Vector2u texSize = texture->getSize();
    sprite.setOrigin(sf::Vector2f(texSize.x / 2.0f, texSize.y / 2.0f));
    sprite.setScale(sf::Vector2f(
        wDraw / (float)texSize.x,
        hDraw / (float)texSize.y
    ));
    
    float angle = std::atan2(tank->dirY, tank->dirX) * 180.0f / 3.14159265f;
    sprite.setRotation(sf::degrees(angle + 90.0f));
    
    window.draw(sprite);
    return true;
}

void Renderer::drawTankBuiltin(sf::RenderWindow& window, Tank* tank,
                               sf::Color color,
                               int wDraw, int hDraw, int xDraw, int yDraw) {
    float radius = 4.0f;
    float cornerRadius = std::min(radius, std::min(wDraw, hDraw) / 2.0f);
    sf::ConvexShape body = createRoundedRect(xDraw, yDraw, wDraw, hDraw, cornerRadius, color);
    body.setOutlineColor(tank->isProtected() ? sf::Color::White : sf::Color::White);
    body.setOutlineThickness(tank->isProtected() ? 3.0f : 1.0f);
    window.draw(body);

    sf::Vector2f center = tank->getCenter();
    sf::CircleShape turret(tank->w / 5.0f);
    turret.setPosition(sf::Vector2f(center.x - tank->w/5.0f, center.y - tank->h/5.0f));
    turret.setFillColor(sf::Color::White);
    window.draw(turret);

    float endX = center.x + tank->dirX * (tank->w / 2.0f + 2.0f);
    float endY = center.y + tank->dirY * (tank->h / 2.0f + 2.0f);
    float angle = std::atan2(tank->dirY, tank->dirX) * 180.0f / 3.14159265f;
    sf::RectangleShape barrel(sf::Vector2f(tank->w / 2.0f + 2.0f, 4.0f));
    barrel.setOrigin(sf::Vector2f(0, 2.0f));
    barrel.setPosition(center);
    barrel.setRotation(sf::degrees(angle));
    barrel.setFillColor(sf::Color::White);
    window.draw(barrel);
}

// ============================================================
// Boss 血条
// ============================================================

void Renderer::drawBossHealthBar(sf::RenderWindow& window, Tank* tank, int maxHp) {
    sf::Vector2f center = tank->getCenter();
    int barWidth = tank->w + 20;
    int barHeight = 8;
    int barX = static_cast<int>(center.x - barWidth / 2.0f);
    int barY = static_cast<int>(tank->y - 16);

    float hpRatio = static_cast<float>(tank->lives) / maxHp;

    sf::RectangleShape barBg(sf::Vector2f(barWidth, barHeight));
    barBg.setPosition(sf::Vector2f(barX, barY));
    barBg.setFillColor(sf::Color(40, 40, 50));
    window.draw(barBg);

    sf::Color hpColor;
    if (hpRatio > 0.5f) hpColor = sf::Color(0, 200, 0);
    else if (hpRatio > 0.25f) hpColor = sf::Color(200, 200, 0);
    else hpColor = sf::Color(200, 50, 50);

    sf::RectangleShape barHp(sf::Vector2f(
        static_cast<int>(barWidth * hpRatio),
        barHeight
    ));
    barHp.setPosition(sf::Vector2f(barX, barY));
    barHp.setFillColor(hpColor);
    window.draw(barHp);

    sf::RectangleShape border(sf::Vector2f(barWidth, barHeight));
    border.setPosition(sf::Vector2f(barX, barY));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(255, 215, 0));
    border.setOutlineThickness(1.0f);
    window.draw(border);
}

// ============================================================
// 子弹绘制
// ============================================================

bool Renderer::drawBulletWithTexture(sf::RenderWindow& window, Bullet* bullet,
                                     const std::string& entityId) {
    auto texture = TextureManager::getInstance().getEntityTexture(entityId);
    if (!texture) return false;

    int cx = static_cast<int>(bullet->x + bullet->w / 2.0f);
    int cy = static_cast<int>(bullet->y + bullet->h / 2.0f);

    sf::Sprite sprite(*texture);
    sprite.setPosition(sf::Vector2f(cx - bullet->w/2.0f, cy - bullet->h/2.0f));
    sprite.setScale(sf::Vector2f(
        bullet->w / (float)texture->getSize().x,
        bullet->h / (float)texture->getSize().y
    ));
    window.draw(sprite);
    return true;
}

void Renderer::drawBulletBuiltin(sf::RenderWindow& window, Bullet* bullet) {
    int cx = static_cast<int>(bullet->x + bullet->w / 2.0f);
    int cy = static_cast<int>(bullet->y + bullet->h / 2.0f);

    sf::CircleShape shape(bullet->w / 2.0f);
    shape.setPosition(sf::Vector2f(cx - bullet->w/2.0f, cy - bullet->h/2.0f));
    shape.setFillColor(bullet->color);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(1.0f);
    window.draw(shape);

    sf::CircleShape highlight(bullet->w / 4.0f);
    highlight.setPosition(sf::Vector2f(
        cx - bullet->w/4.0f - 1.0f,
        cy - bullet->h/4.0f - 1.0f
    ));
    highlight.setFillColor(sf::Color(255, 255, 255, 180));
    window.draw(highlight);
}

// ============================================================
// 墙壁绘制
// ============================================================

bool Renderer::drawWallWithTexture(sf::RenderWindow& window, Wall* wall,
                                   const std::string& entityId) {
    auto texture = TextureManager::getInstance().getEntityTexture(entityId);
    if (!texture) return false;

    sf::Sprite sprite(*texture);
    sprite.setPosition(sf::Vector2f(wall->x, wall->y));
    sprite.setScale(sf::Vector2f(
        wall->w / (float)texture->getSize().x,
        wall->h / (float)texture->getSize().y
    ));
    window.draw(sprite);
    return true;
}

void Renderer::drawWallBuiltin(sf::RenderWindow& window, Wall* wall) {
    sf::Color fillColor = wall->isSteel() ? COLOR_STEEL : COLOR_WALL;
    float radius = 4.0f;
    sf::ConvexShape wallShape = createRoundedRect(wall->x, wall->y, wall->w, wall->h, radius, fillColor);
    window.draw(wallShape);
}

// ============================================================
// 道具绘制
// ============================================================

bool Renderer::drawPowerUpWithTexture(sf::RenderWindow& window, PowerUp* powerup,
                                     const std::string& entityId) {
    auto texture = TextureManager::getInstance().getEntityTexture(entityId);
    if (!texture) return false;

    sf::Sprite sprite(*texture);
    sprite.setPosition(sf::Vector2f(powerup->getX(), powerup->getY()));
    sprite.setScale(sf::Vector2f(
        powerup->getW() / (float)texture->getSize().x,
        powerup->getH() / (float)texture->getSize().y
    ));
    window.draw(sprite);
    return true;
}

void Renderer::drawPowerUpBuiltin(sf::RenderWindow& window, PowerUp* powerup) {
    sf::Color color;
    switch (powerup->getType()) {
        case 'S': color = sf::Color(50, 150, 255); break;
        case 'P': color = sf::Color::White; break;
        case 'H': color = sf::Color(255, 50, 50); break;
        case 'T': color = sf::Color(255, 150, 50); break;
        default: color = sf::Color::White;
    }

    sf::RectangleShape rect(sf::Vector2f(powerup->getW(), powerup->getH()));
    rect.setPosition(sf::Vector2f(powerup->getX(), powerup->getY()));
    rect.setFillColor(color);
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(1.0f);
    window.draw(rect);

    // 文字
    sf::Font font;
    if (font.openFromFile("assets/fonts/arial.ttf") ||
        font.openFromFile("assets/fonts/consola.ttf")) {
        sf::Text text(font, std::string(1, powerup->getType()), 16);
        text.setFillColor(sf::Color::Black);
        text.setStyle(sf::Text::Bold);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setPosition(sf::Vector2f(
            powerup->getX() + powerup->getW()/2.0f - bounds.size.x/2.0f,
            powerup->getY() + powerup->getH()/2.0f - bounds.size.y/2.0f - 2.0f
        ));
        window.draw(text);
    }
}