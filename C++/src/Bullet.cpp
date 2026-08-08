#include "Bullet.h"
#include "resource/TextureManager.h"

Bullet::Bullet(float x, float y, float dx, float dy, bool isPlayer, int pid, sf::Color col)
    : x(x - getBulletSize()/2.0f), y(y - getBulletSize()/2.0f),
      w(getBulletSize()), h(getBulletSize()),
      dx(dx), dy(dy), speed(getBulletSpeed()),
      player(isPlayer), playerId(pid), color(col), alive(true), damage(1) {}

void Bullet::update(float dt) {
    // 添加拖尾粒子
    TrailParticle p;
    p.x = x + w/2;
    p.y = y + h/2;
    p.size = w * 0.6f;
    p.age = 0;
    p.lifetime = 0.15f;
    trail.push_back(p);

    // 更新位置
    x += dx * speed * dt;
    y += dy * speed * dt;

    // 更新拖尾
    for (auto it = trail.begin(); it != trail.end(); ) {
        it->age += dt;
        it->size *= (1.0f - dt / it->lifetime);
        if (it->age >= it->lifetime || it->size < 0.5f) {
            it = trail.erase(it);
        } else {
            ++it;
        }
    }

    if (x < -20 || x > WINDOW_WIDTH + 20 || y < -20 || y > WINDOW_HEIGHT + 20) {
        alive = false;
    }
}

void Bullet::draw(sf::RenderWindow& window) {
    // 画拖尾
    for (const auto& p : trail) {
        float alpha = 1.0f - p.age / p.lifetime;
        int size = static_cast<int>(p.size);
        if (size > 0 && alpha > 0.05f) {
            sf::CircleShape shape(size);
            shape.setPosition(sf::Vector2f(p.x - size, p.y - size));
            sf::Color col = color;
            col.a = static_cast<uint8_t>(alpha * 255);
            shape.setFillColor(col);
            window.draw(shape);
        }
    }

    if (!alive) return;

    // ===== 尝试获取贴图 =====
    std::string entityId;
    if (player) {
        entityId = "bullet_p" + std::to_string(playerId);
    } else {
        entityId = "bullet_enemy";
    }

    auto& tm = TextureManager::getInstance();
    auto texture = tm.getEntityTexture(entityId);

    int cx = static_cast<int>(x + w / 2.0f);
    int cy = static_cast<int>(y + h / 2.0f);

    if (texture) {
        // ===== 使用贴图绘制 =====
        sf::Sprite sprite(*texture);
        sprite.setPosition(sf::Vector2f(cx - w/2.0f, cy - h/2.0f));
        sprite.setScale(sf::Vector2f(
            w / (float)texture->getSize().x,
            h / (float)texture->getSize().y
        ));
        window.draw(sprite);
        return;
    }

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