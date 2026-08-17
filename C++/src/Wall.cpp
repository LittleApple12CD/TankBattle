#include "Wall.h"
#include "resource/TextureManager.h"
#include "renderer/Renderer.h"

Wall::Wall(float x, float y, float w, float h, bool isSteel)
    : x(x), y(y), w(w), h(h), steel(isSteel), alive(true) {}

sf::FloatRect Wall::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}

void Wall::draw(sf::RenderWindow& window) {
    if (!alive) return;

    std::string entityId = steel ? "wall_steel" : "wall_brick";
    Renderer& renderer = Renderer::getInstance();

    // ===== 尝试使用贴图 =====
    if (renderer.drawWallWithTexture(window, this, entityId)) {
        return;
    }

    renderer.drawWallBuiltin(window, this);
}