#include "Wall.h"
#include "resource/TextureManager.h"

Wall::Wall(float x, float y, float w, float h, bool isSteel)
    : x(x), y(y), w(w), h(h), steel(isSteel), alive(true) {}

sf::FloatRect Wall::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}

void Wall::draw(sf::RenderWindow& window) {
    if (!alive) return;

    // ===== 尝试获取贴图 =====
    std::string entityId = steel ? "wall_steel" : "wall_brick";
    auto& tm = TextureManager::getInstance();
    auto texture = tm.getEntityTexture(entityId);

    if (texture) {
        // ===== 使用贴图绘制 =====
        sf::Sprite sprite(*texture);
        sprite.setPosition(sf::Vector2f(x, y));
        sprite.setScale(sf::Vector2f(
            w / (float)texture->getSize().x,
            h / (float)texture->getSize().y
        ));
        window.draw(sprite);
        return;
    }

    sf::Color fillColor = steel ? COLOR_STEEL : COLOR_WALL;
    float radius = 4.0f;
    sf::ConvexShape wall = createRoundedRect(x, y, w, h, radius, fillColor);
    window.draw(wall);
}