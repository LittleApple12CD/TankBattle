#include "Boss.h"
#include "Utils.h"
#include "resource/TextureManager.h"
#include "renderer/Renderer.h"
#include <cmath>

Boss::Boss(float x, float y, int hp, double sizeMult, double speedMult,
           int bulletDamage, double bulletSpeedMult)
    : Tank(x, y, sf::Color(200, 50, 200), TANK_SPEED * speedMult, false, 0),
      maxHp(hp), sizeMult(sizeMult), speedMult(speedMult),
      bulletDamage(bulletDamage), bulletSpeedMult(bulletSpeedMult) {

    this->isBoss = true;
    this->lives = hp;
    this->w = static_cast<int>(TANK_SIZE * sizeMult);
    this->h = static_cast<int>(TANK_SIZE * sizeMult);
    this->x = x;
    this->y = y;
}

sf::Vector2f Boss::getFirePoint() const {
    sf::Vector2f center = getCenter();
    float offset = w / 2.0f + 2.0f;
    return sf::Vector2f(center.x + dirX * offset, center.y + dirY * offset);
}

Bullet* Boss::shoot() {
    if (cooldown > 0) return nullptr;
    if (bullets.size() >= MAX_BULLETS) return nullptr;

    cooldown = SHOT_COOLDOWN * 0.7f;
    sf::Vector2f firePoint = getFirePoint();

    int bulletSize = static_cast<int>(BULLET_SIZE * 1.5);
    float bulletSpeed = BULLET_SPEED * static_cast<float>(bulletSpeedMult);

    // 创建 Boss 子弹
    bullets.emplace_back(firePoint.x, firePoint.y, dirX, dirY, false, 0, COLOR_BULLET_ENEMY);
    Bullet& bullet = bullets.back();
    bullet.w = bulletSize;
    bullet.h = bulletSize;
    bullet.speed = bulletSpeed;
    bullet.damage = bulletDamage;
    bullet.color = COLOR_BULLET_ENEMY;
    return &bullet;
}

void Boss::draw(sf::RenderWindow& window) {
    if (!alive) return;

    float scale = getSizeScale();
    int wDraw = static_cast<int>(w * scale);
    int hDraw = static_cast<int>(h * scale);
    int xDraw = static_cast<int>(x - (wDraw - w) / 2.0f);
    int yDraw = static_cast<int>(y - (hDraw - h) / 2.0f);

    Renderer& renderer = Renderer::getInstance();

    // ===== 尝试使用贴图 =====
    if (renderer.drawTankWithTexture(window, this, "tank_boss", wDraw, hDraw, xDraw, yDraw)) {
        renderer.drawBossHealthBar(window, this, maxHp);
        return;
    }

    // ===== 无贴图：回退到内置绘制 =====
    sf::Vector2f center = getCenter();
    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);

    float radius = 6.0f;
    sf::ConvexShape body = createRoundedRect(xDraw, yDraw, w, h, radius,
                                              sf::Color(180, 50, 200),
                                              sf::Color(255, 215, 0),
                                              3.0f);
    window.draw(body);

    sf::Font font;
    if (font.openFromFile("C:/Windows/Fonts/Arial.ttf") ||
        font.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        sf::Text star(font, "*", 24);
        star.setFillColor(sf::Color(255, 215, 0));
        star.setPosition(sf::Vector2f(cx - 12, cy - 14));
        window.draw(star);
    }

    sf::CircleShape turret(w / 6.0f);
    turret.setPosition(sf::Vector2f(cx - w/6.0f, cy - h/6.0f));
    turret.setFillColor(sf::Color(255, 215, 0));
    window.draw(turret);

    float endX = cx + dirX * (w / 2.0f + 2.0f);
    float endY = cy + dirY * (h / 2.0f + 2.0f);
    sf::Vertex line[2];
    line[0].position = sf::Vector2f(cx, cy);
    line[0].color = sf::Color(255, 215, 0);
    line[1].position = sf::Vector2f(endX, endY);
    line[1].color = sf::Color(255, 215, 0);
    window.draw(line, 2, sf::PrimitiveType::Lines);

    renderer.drawBossHealthBar(window, this, maxHp);
}