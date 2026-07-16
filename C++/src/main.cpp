#include <SFML/Graphics.hpp>
#include "Game.h"

int main() {
    sf::VideoMode vm(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
    sf::RenderWindow window(vm, "TankBattle - C++");
    window.setFramerateLimit(FPS);

    // 设置窗口图标
    sf::Image icon;
    if (icon.loadFromFile("icon.png")) {
        window.setIcon(sf::Vector2u(icon.getSize().x, icon.getSize().y), icon.getPixelsPtr());
    }

    Game game;
    sf::Clock clock;

    bool p1Left=false, p1Right=false, p1Up=false, p1Down=false, p1Shoot=false;
    bool p2Left=false, p2Right=false, p2Up=false, p2Down=false, p2Shoot=false;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        while (auto eventOpt = window.pollEvent()) {
            sf::Event event = *eventOpt;
            if (event.is<sf::Event::Closed>()) window.close();

            if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Escape: window.close(); break;
                    case sf::Keyboard::Key::P: game.setPaused(!game.isPaused()); break;
                    case sf::Keyboard::Key::R: game.initLevel(); break;
                    case sf::Keyboard::Key::G: game.togglePvpMode(); break;
                    case sf::Keyboard::Key::Left: p1Left = true; break;
                    case sf::Keyboard::Key::Right: p1Right = true; break;
                    case sf::Keyboard::Key::Up: p1Up = true; break;
                    case sf::Keyboard::Key::Down: p1Down = true; break;
                    case sf::Keyboard::Key::Space: p1Shoot = true; break;
                    case sf::Keyboard::Key::A: p2Left = true; break;
                    case sf::Keyboard::Key::D: p2Right = true; break;
                    case sf::Keyboard::Key::W: p2Up = true; break;
                    case sf::Keyboard::Key::S: p2Down = true; break;
                    case sf::Keyboard::Key::J: p2Shoot = true; break;
                    case sf::Keyboard::Key::O: game.toggleSingleMode(); break;
                    default: break;
                }
            }

            if (auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
                switch (keyReleased->code) {
                    case sf::Keyboard::Key::Left: p1Left = false; break;
                    case sf::Keyboard::Key::Right: p1Right = false; break;
                    case sf::Keyboard::Key::Up: p1Up = false; break;
                    case sf::Keyboard::Key::Down: p1Down = false; break;
                    case sf::Keyboard::Key::Space: p1Shoot = false; break;
                    case sf::Keyboard::Key::A: p2Left = false; break;
                    case sf::Keyboard::Key::D: p2Right = false; break;
                    case sf::Keyboard::Key::W: p2Up = false; break;
                    case sf::Keyboard::Key::S: p2Down = false; break;
                    case sf::Keyboard::Key::J: p2Shoot = false; break;
                    default: break;
                }
            }
        }

        if (!game.isPaused() && !game.isGameOver()) {
            int dx1=0, dy1=0;
            if (p1Left) dx1=-1;
            else if (p1Right) dx1=1;
            else if (p1Up) dy1=-1;
            else if (p1Down) dy1=1;
            if (dx1!=0 || dy1!=0) game.movePlayer1(dx1, dy1);
            if (p1Shoot) { game.player1Shoot(); p1Shoot = false; }

            int dx2=0, dy2=0;
            if (p2Left) dx2=-1;
            else if (p2Right) dx2=1;
            else if (p2Up) dy2=-1;
            else if (p2Down) dy2=1;
            if (dx2!=0 || dy2!=0) game.movePlayer2(dx2, dy2);
            if (p2Shoot) { game.player2Shoot(); p2Shoot = false; }
        }

        game.update(dt);
        game.draw(window);
        window.display();
    }
    return 0;
}