#include "Tank.h"
#include <cmath>

Tank::Tank(float x, float y, sf::Color color, float speed, bool isPlayer, int pid)
    : x(x), y(y), w(TANK_SIZE), h(TANK_SIZE), color(color), speed(speed),
      player(isPlayer), playerId(pid), dirX(0), dirY(-1),
      lives(isPlayer ? PLAYER_LIVES : 1), cooldown(0), alive(true) {}

void Tank::update(float dt) {
    if (cooldown > 0) cooldown -= dt;
    
    // list 遍历
    for (auto it = bullets.begin(); it != bullets.end(); ) {
        it->update(dt);
        if (!it->isAlive()) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void Tank::draw(sf::RenderWindow& window) {
    if (!alive) return;

    // 主体
    sf::ConvexShape body = create16Shape(x, y, w, h, 4.0f, 12.0f, color);
    window.draw(body);

    // 边框
    sf::ConvexShape border = create16Border(x, y, w, h, 4.0f, 12.0f);
    window.draw(border);

    // 炮塔
    sf::Vector2f center = getCenter();
    sf::CircleShape turret(w / 5.0f);
    turret.setPosition(sf::Vector2f(center.x - w/5.0f, center.y - h/5.0f));
    turret.setFillColor(sf::Color::White);
    window.draw(turret);

    // 炮管
    float endX = center.x + dirX * (w / 2.0f + 2.0f);
    float endY = center.y + dirY * (h / 2.0f + 2.0f);
    sf::Vertex line[2];
    line[0].position = sf::Vector2f(center.x, center.y);
    line[0].color = sf::Color::White;
    line[1].position = sf::Vector2f(endX, endY);
    line[1].color = sf::Color::White;
    window.draw(line, 2, sf::PrimitiveType::Lines);

    // 玩家编号
    if (player) {
        sf::Font font;
        if (font.openFromFile("C:/Windows/Fonts/Arial.ttf") ||
            font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            sf::Text text(font, std::to_string(playerId), 14);
            text.setFillColor(sf::Color::Black);
            text.setStyle(sf::Text::Bold);
            sf::FloatRect bounds = text.getLocalBounds();
            text.setPosition(sf::Vector2f(center.x - bounds.size.x/2.0f,
                                          center.y - bounds.size.y/2.0f - 2));
            window.draw(text);
        }
    }
    
    // 子弹
    for (auto& b : bullets) {
        b.draw(window);
    }
}

void Tank::move(int dx, int dy, std::vector<Wall>& walls) {
    if (dx == 0 && dy == 0) return;
    
    float step = MOVE_STEP;
    float stepDx = dx * step;
    float stepDy = dy * step;
    int totalSteps = static_cast<int>(speed / step);
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

void Tank::shoot() {
    if (cooldown > 0) return;
    if (bullets.size() >= MAX_BULLETS) return;
    
    cooldown = SHOT_COOLDOWN;
    sf::Vector2f firePoint = getFirePoint();
    sf::Color bulletColor;
    if (player) {
        bulletColor = (playerId == 1) ? COLOR_BULLET_P1 : COLOR_BULLET_P2;
    } else {
        bulletColor = COLOR_BULLET_ENEMY;
    }
    bullets.emplace_back(firePoint.x, firePoint.y, dirX, dirY, player, playerId, bulletColor);
}

sf::FloatRect Tank::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}

sf::Vector2f Tank::getCenter() const {
    return sf::Vector2f(x + w/2.0f, y + h/2.0f);
}

sf::Vector2f Tank::getFirePoint() const {
    sf::Vector2f center = getCenter();
    float offset = w / 2.0f + 2.0f;
    return sf::Vector2f(center.x + dirX * offset, center.y + dirY * offset);
}