#include "PowerUp.h"
#include "resource/TextureManager.h"
#include "renderer/Renderer.h"

PowerUp::PowerUp(float x, float y, char type)
    : x(x), y(y), w(24), h(24), type(type), alive(true) {
    bool loaded = font.openFromFile("C:/Windows/Fonts/Arial/arial.ttf");
    if (!loaded) {
        loaded = font.openFromFile("C:/Windows/Fonts/consola.ttf");
    }
}

sf::Color PowerUp::getColor() const {
    switch (type) {
        case 'S': return sf::Color(50, 150, 255);
        case 'P': return sf::Color::White;
        case 'H': return sf::Color(255, 50, 50);
        case 'T': return sf::Color(255, 150, 50);
        default: return sf::Color::White;
    }
}

void PowerUp::draw(sf::RenderWindow& window) {
    if (!alive) return;

    std::string entityId = "powerup_" + std::string(1, type);
    Renderer& renderer = Renderer::getInstance();

    // ===== 尝试使用贴图 =====
    if (renderer.drawPowerUpWithTexture(window, this, entityId)) {
        return;
    }

    renderer.drawPowerUpBuiltin(window, this);
}

sf::FloatRect PowerUp::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}