// src/renderer/Renderer.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class Tank;
class Bullet;
class Wall;
class PowerUp;

/**
 * 轻量级渲染器 - 只负责贴图实体的绘制
 * 不包含 UI、HUD、爆炸、痕迹等
 */
class Renderer {
public:
    static Renderer& getInstance();

    // ===== 坦克绘制 =====
    bool drawTankWithTexture(sf::RenderWindow& window, Tank* tank, 
                             const std::string& entityId,
                             int wDraw, int hDraw, int xDraw, int yDraw);
    void drawTankBuiltin(sf::RenderWindow& window, Tank* tank,
                         sf::Color color,
                         int wDraw, int hDraw, int xDraw, int yDraw);

    // ===== Boss 血条 =====
    void drawBossHealthBar(sf::RenderWindow& window, Tank* tank, int maxHp);

    // ===== 子弹绘制 =====
    bool drawBulletWithTexture(sf::RenderWindow& window, Bullet* bullet,
                               const std::string& entityId);
    void drawBulletBuiltin(sf::RenderWindow& window, Bullet* bullet);

    // ===== 墙壁绘制 =====
    bool drawWallWithTexture(sf::RenderWindow& window, Wall* wall,
                             const std::string& entityId);
    void drawWallBuiltin(sf::RenderWindow& window, Wall* wall);

    // ===== 道具绘制 =====
    bool drawPowerUpWithTexture(sf::RenderWindow& window, PowerUp* powerup,
                                const std::string& entityId);
    void drawPowerUpBuiltin(sf::RenderWindow& window, PowerUp* powerup);

private:
    Renderer() = default;
    ~Renderer() = default;
};