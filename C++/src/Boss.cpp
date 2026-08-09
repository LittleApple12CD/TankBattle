#include "Boss.h"
#include "Utils.h"
#include "resource/TextureManager.h"
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

    // ===== 尝试获取贴图 =====
    auto& tm = TextureManager::getInstance();
    auto texture = tm.getEntityTexture("tank_boss");

    if (texture) {
        // ===== 使用贴图绘制并旋转 =====
        sf::Sprite sprite(*texture);
        
        sf::Vector2f center = getCenter();
        sprite.setPosition(center);
        
        sf::Vector2u texSize = texture->getSize();
        sprite.setOrigin(sf::Vector2f(texSize.x / 2.0f, texSize.y / 2.0f));
        
        sprite.setScale(sf::Vector2f(
            wDraw / (float)texSize.x,
            hDraw / (float)texSize.y
        ));
        
        float angle = std::atan2(dirY, dirX) * 180.0f / 3.14159265f;
        sprite.setRotation(sf::degrees(angle + 90.0f));
        
        window.draw(sprite);

        // ===== Boss 血条 =====
        drawBossHealthBar(window);
        return;
    }

    sf::Vector2f center = getCenter();
    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);

    // ===== Boss 主体 =====
    float radius = 6.0f;
    sf::ConvexShape body = createRoundedRect(
        static_cast<float>(xDraw),
        static_cast<float>(yDraw),
        static_cast<float>(w),
        static_cast<float>(h),
        radius,
        sf::Color(180, 50, 200),
        sf::Color(255, 215, 0),
        3.0f
    );
    window.draw(body);

    // ===== 炮塔 =====
    sf::CircleShape turret(w / 6.0f);
    turret.setPosition(sf::Vector2f(cx - w/6.0f, cy - h/6.0f));
    turret.setFillColor(sf::Color(255, 215, 0));
    window.draw(turret);

    // ===== 炮管 =====
    float endX = cx + dirX * (w / 2.0f + 2.0f);
    float endY = cy + dirY * (h / 2.0f + 2.0f);
    sf::Vertex line[2];
    line[0].position = sf::Vector2f(cx, cy);
    line[0].color = sf::Color(255, 215, 0);
    line[1].position = sf::Vector2f(endX, endY);
    line[1].color = sf::Color(255, 215, 0);
    window.draw(line, 2, sf::PrimitiveType::Lines);

    // ===== 血条 =====
    int barWidth = w + 10;
    int barHeight = 6;
    int barX = cx - barWidth / 2;
    int barY = yDraw - 12;
    float hpRatio = static_cast<float>(lives) / maxHp;

    sf::RectangleShape barBg(sf::Vector2f(barWidth, barHeight));
    barBg.setPosition(sf::Vector2f(barX, barY));
    barBg.setFillColor(sf::Color(60, 60, 60));
    window.draw(barBg);

    sf::Color hpColor;
    if (hpRatio > 0.5f) hpColor = sf::Color(0, 200, 0);
    else if (hpRatio > 0.25f) hpColor = sf::Color(200, 200, 0);
    else hpColor = sf::Color(200, 50, 50);

    sf::RectangleShape barHp(sf::Vector2f(static_cast<int>(barWidth * hpRatio), barHeight));
    barHp.setPosition(sf::Vector2f(barX, barY));
    barHp.setFillColor(hpColor);
    window.draw(barHp);
}