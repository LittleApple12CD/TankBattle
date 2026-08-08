// src/Tank.cpp
#include "Tank.h"
#include "Utils.h"
#include "ConfigManager.h"
#include "resource/TextureManager.h"
#include <cmath>

// ===== 构造函数 =====
Tank::Tank(float x, float y, sf::Color color, float speed, bool isPlayer, int pid)
    : x(x), y(y), w(getTankSize()), h(getTankSize()),
      color(color), speed(speed),
      player(isPlayer), playerId(pid), dirX(0), dirY(-1),
      lives(isPlayer ? getPlayerLives() : 1), cooldown(0), alive(true), frameCounter(0) {}

// ===== update =====
void Tank::update(float dt) {
    if (cooldown > 0) cooldown -= dt;

    for (auto it = effects.begin(); it != effects.end(); ) {
        it->second -= dt;
        if (it->second <= 0) {
            it = effects.erase(it);
        } else {
            ++it;
        }
    }

    frameCounter++;
    if (frameCounter % 3 == 0 && alive) {
        TrailPoint tp;
        tp.x = x + w/2;
        tp.y = y + h/2;
        tp.age = 0;
        trailPoints.push_back(tp);
    }

    for (auto it = trailPoints.begin(); it != trailPoints.end(); ) {
        it->age += dt;
        if (it->age >= 1.0f) {
            it = trailPoints.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = bullets.begin(); it != bullets.end(); ) {
        it->update(dt);
        if (!it->isAlive()) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

// ===== draw =====
void Tank::draw(sf::RenderWindow& window) {
    // 痕迹
    for (const auto& tp : trailPoints) {
        float alpha = 60.0f * (1.0f - tp.age / 1.0f);
        if (alpha > 5.0f) {
            sf::RectangleShape rect(sf::Vector2f(w, h));
            rect.setPosition(sf::Vector2f(tp.x - w/2.0f, tp.y - h/2.0f));
            rect.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));
            window.draw(rect);
        }
    }

    if (!alive) return;

    float scale = getSizeScale();
    int wDraw = static_cast<int>(w * scale);
    int hDraw = static_cast<int>(h * scale);
    int xDraw = static_cast<int>(x - (wDraw - w) / 2.0f);
    int yDraw = static_cast<int>(y - (hDraw - h) / 2.0f);

    // 尝试获取贴图
    std::string entityId;
    if (player) {
        entityId = "tank_p" + std::to_string(playerId);
    } else if (isBoss) {
        entityId = "tank_boss";
    } else {
        entityId = "tank_enemy";
    }

    auto& tm = TextureManager::getInstance();
    auto texture = tm.getEntityTexture(entityId);

    if (texture) {
        sf::Sprite sprite(*texture);
        sprite.setPosition(sf::Vector2f(xDraw, yDraw));
        sprite.setScale(sf::Vector2f(
            wDraw / (float)texture->getSize().x,
            hDraw / (float)texture->getSize().y
        ));

        float angle = std::atan2(dirY, dirX) * 180.0f / 3.14159265f;
        sprite.setRotation(sf::degrees(angle + 90.0f));
        window.draw(sprite);

        if (isBoss) {
            drawBossHealthBar(window);
        }
        return;
    }

    // 内置绘制
    float radius = 4.0f;
    float cornerRadius = std::min(radius, std::min(wDraw, hDraw) / 2.0f);
    sf::ConvexShape body = createRoundedRect(xDraw, yDraw, wDraw, hDraw, cornerRadius, color);
    body.setOutlineColor(isProtected() ? sf::Color::White : sf::Color::White);
    body.setOutlineThickness(isProtected() ? 3.0f : 1.0f);
    window.draw(body);

    sf::Vector2f center = getCenter();
    sf::CircleShape turret(w / 5.0f);
    turret.setPosition(sf::Vector2f(center.x - w/5.0f, center.y - h/5.0f));
    turret.setFillColor(sf::Color::White);
    window.draw(turret);

    float endX = center.x + dirX * (w / 2.0f + 2.0f);
    float endY = center.y + dirY * (h / 2.0f + 2.0f);
    float angle = atan2(dirY, dirX) * 180.0f / 3.14159f;
    float barrelLength = w / 2.0f + 2.0f;
    float barrelWidth = 4.0f;
    sf::RectangleShape barrel(sf::Vector2f(barrelLength, barrelWidth));
    barrel.setOrigin(sf::Vector2f(0, barrelWidth / 2.0f));
    barrel.setPosition(center);
    barrel.setRotation(sf::degrees(angle));
    barrel.setFillColor(sf::Color::White);
    window.draw(barrel);

    if (isBoss) {
        drawBossHealthBar(window);
    }
}

// ===== drawBossHealthBar =====
void Tank::drawBossHealthBar(sf::RenderWindow& window) {
    if (!isBoss || maxHp <= 0) return;

    sf::Vector2f center = getCenter();
    int barWidth = w + 20;
    int barHeight = 8;
    int barX = static_cast<int>(center.x - barWidth / 2.0f);
    int barY = static_cast<int>(y - 16);

    float hpRatio = static_cast<float>(lives) / maxHp;

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

// ===== move =====
void Tank::move(int dx, int dy, std::vector<Wall>& walls) {
    if (dx == 0 && dy == 0) return;
    
    float currentSpeed = speed * getSpeedMultiplier();
    float step = MOVE_STEP;
    float stepDx = dx * step;
    float stepDy = dy * step;
    int totalSteps = static_cast<int>(currentSpeed / step);
    if (totalSteps < 1) totalSteps = 1;

    for (int i = 0; i < totalSteps; ++i) {
        float newX = x + stepDx;
        float newY = y + stepDy;
        int margin = 2;
        float maxX = WINDOW_WIDTH - w - margin;
        float maxY = WINDOW_HEIGHT - h - margin;
        newX = clampf(newX, margin, maxX);
        newY = clampf(newY, margin, maxY);

        sf::FloatRect testRect(sf::Vector2f(newX, newY), sf::Vector2f(w, h));
        bool blocked = false;
        for (auto& wall : walls) {
            if (wall.isAlive() && testRect.findIntersection(wall.getRect()).has_value()) {
                blocked = true;
                break;
            }
        }
        
        if (!blocked) {
            x = newX;
            y = newY;
        } else {
            if (dx != 0) {
                sf::FloatRect testX(sf::Vector2f(newX, y), sf::Vector2f(w, h));
                bool blockedX = false;
                for (auto& wall : walls) {
                    if (wall.isAlive() && testX.findIntersection(wall.getRect()).has_value()) {
                        blockedX = true;
                        break;
                    }
                }
                if (!blockedX) x = newX;
            }
            if (dy != 0) {
                sf::FloatRect testY(sf::Vector2f(x, newY), sf::Vector2f(w, h));
                bool blockedY = false;
                for (auto& wall : walls) {
                    if (wall.isAlive() && testY.findIntersection(wall.getRect()).has_value()) {
                        blockedY = true;
                        break;
                    }
                }
                if (!blockedY) y = newY;
            }
            break;
        }
    }
}

// ===== shoot =====
Bullet* Tank::shoot() {
    if (cooldown > 0) return nullptr;
    if (bullets.size() >= MAX_BULLETS) return nullptr;

    cooldown = SHOT_COOLDOWN;
    sf::Vector2f firePoint = getFirePoint();

    float bulletSpeed = BULLET_SPEED * getBulletSpeedMultiplier();
    int bulletSize = static_cast<int>(BULLET_SIZE * getSizeScale());
    int bulletDamage = getBulletDamage();

    sf::Color bulletColor;
    if (player) {
        bulletColor = (playerId == 1) ? COLOR_BULLET_P1 : COLOR_BULLET_P2;
    } else {
        bulletColor = COLOR_BULLET_ENEMY;
    }

    bullets.emplace_back(firePoint.x, firePoint.y, dirX, dirY, player, playerId, bulletColor);
    Bullet& bullet = bullets.back();
    bullet.speed = bulletSpeed;
    bullet.w = bulletSize;
    bullet.h = bulletSize;
    bullet.damage = bulletDamage;
    return &bullet;
}

// ===== getRect =====
sf::FloatRect Tank::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}

// ===== getCenter =====
sf::Vector2f Tank::getCenter() const {
    return sf::Vector2f(x + w/2.0f, y + h/2.0f);
}

// ===== getFirePoint =====
sf::Vector2f Tank::getFirePoint() const {
    sf::Vector2f center = getCenter();
    float offset = w / 2.0f + 2.0f;
    return sf::Vector2f(center.x + dirX * offset, center.y + dirY * offset);
}