#include "PowerUp.h"

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

    sf::Color color = getColor();
    sf::RectangleShape rect(sf::Vector2f(w, h));
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(color);
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(1.0f);
    window.draw(rect);

    sf::Text text(font, std::string(1, type), 16);
    text.setFillColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition(sf::Vector2f(x + w/2.0f - bounds.size.x/2.0f,
                                  y + h/2.0f - bounds.size.y/2.0f - 2.0f));
    window.draw(text);
}

sf::FloatRect PowerUp::getRect() const {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}